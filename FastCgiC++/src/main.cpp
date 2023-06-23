#include "main.h"

//-----------------------------------------------------------------------------

struct memory
{
  char *response;
  size_t size;
};

vector<string> vec;

#define port  80

CURL *curlVector;
CURL *curlStatus;
CURLcode res;
#define SIZE_VEC  10
#define SIZE_REQUEST  80

char requestAddr[SIZE_REQUEST] = {0};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static size_t writeCallbackForAllData
(
    void *data, size_t size,
    size_t nmemb, void *clientp)
{

  size_t realsize = size * nmemb;
  struct memory *mem = static_cast<struct memory *>(clientp);

  char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
  if(ptr == NULL)
  {
    return 0;  // out of memory!
  }

  string s;
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  int sizeStr = strlen(mem->response);

  s = mem->response;
  vec.push_back(s);

  //printf("rx data from ESP32 : %d / %d\n", vec.size(), SIZE_VEC);
  
  cout << "address array : " << requestAddr << endl;
/*
  struct memory chunkReadBuffer = {0};
  
  curlStatus = curl_easy_init();

  if(curlStatus)
  {
    curl_easy_setopt(curlStatus, CURLOPT_URL, static_cast<void *>(requestAddr));
    // example.com is redirected, so we tell libcurl to follow redirection
    curl_easy_setopt(curlStatus, CURLOPT_FOLLOWLOCATION, 1L);
    //CURLOPT_WRITEFUNCTION - callback for writing received data
    curl_easy_setopt(curlVector, CURLOPT_WRITEFUNCTION , writeCallbackItem);
    //CURLOPT_WRITEDATA - pointer passed to the write callback
    curl_easy_setopt(curlStatus, CURLOPT_WRITEDATA, (void *)&chunkReadBuffer);
    //
    res = curl_easy_perform(curlStatus);
    // always cleanup
    curl_easy_cleanup(curlStatus);
  }
*/

  memset(mem->response, 0, sizeStr);
  mem->size = 0;

  return realsize;
}

//-----------------------------------------------------------------------------

void *threadHttpRequestFunc(void *ipAddrToESP)
{
  struct memory chunkReadBuffer = {0};
  
  snprintf(requestAddr, SIZE_REQUEST, "%s", static_cast<char *>(ipAddrToESP));
  //printf("request %s\n", static_cast<char *>(ipAddrToESP));

  curlVector = curl_easy_init();

  if(curlVector)
  {
    curl_easy_setopt(curlVector, CURLOPT_URL, static_cast<void *>(requestAddr));
    // example.com is redirected, so we tell libcurl to follow redirection
    curl_easy_setopt(curlVector, CURLOPT_FOLLOWLOCATION, 1L);
    //CURLOPT_WRITEFUNCTION - callback for writing received data
    curl_easy_setopt(curlVector, CURLOPT_WRITEFUNCTION , writeCallbackForAllData);
    //CURLOPT_WRITEDATA - pointer passed to the write callback
    curl_easy_setopt(curlVector, CURLOPT_WRITEDATA, (void *)&chunkReadBuffer);
  }
  else
  {
    pthread_exit(0);
  }

  uint16_t i;
  uint32_t delay = 500 * 1000;  //

  vec.clear();

  for(i = 0; i < SIZE_VEC; i++)
  {
    // Perform the request, res will get the return code
    res = curl_easy_perform(curlVector);
    // Check for errors
    if(res != CURLE_OK)
    {
      printf("curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
              
      pthread_exit(0);
      
    }
    usleep(delay);
  }
  //
  cout << "vector<string> print on console" << endl;
  for(vector<string>::iterator it = vec.begin();
      it != vec.end();
      it++)
  {
    cout << *it << endl;
  }

  // always cleanup
  curl_easy_cleanup(curlVector);

  free(chunkReadBuffer.response);

  pthread_exit(0);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

string sTx;

//-----------------------------------------------------------------------------

static size_t writeCallbackItem
(
    void *data, size_t size,
    size_t nmemb, void *clientp)
{
  

  size_t realsize = size * nmemb;
  struct memory *mem = static_cast<struct memory *>(clientp);

  char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
  if(ptr == NULL)
  {
    return 0;  // out of memory!
  }

  string s;
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  int sizeStr = strlen(mem->response);

  s = mem->response;
  vec.push_back(s);

  //printf("rx data from ESP32 : %d / %d\n", vec.size(), SIZE_VEC);
  
  cout << "address array : " << requestAddr << endl;

  memset(mem->response, 0, sizeStr);
  mem->size = 0;

  return realsize;
}


void *threadOneReqFunc(void *ipAddrToESP)
{
  struct memory chunkReadBuffer = {0};
  
  snprintf(requestAddr, SIZE_REQUEST, "%s", static_cast<char *>(ipAddrToESP));
  //printf("request %s\n", static_cast<char *>(ipAddrToESP));

  curlVector = curl_easy_init();
  
  vec.clear();

  if(curlVector)
  {
    curl_easy_setopt(curlVector, CURLOPT_URL, static_cast<void *>(requestAddr));
    // example.com is redirected, so we tell libcurl to follow redirection
    curl_easy_setopt(curlVector, CURLOPT_FOLLOWLOCATION, 1L);
    //CURLOPT_WRITEFUNCTION - callback for writing received data
    curl_easy_setopt(curlVector, CURLOPT_WRITEFUNCTION , writeCallbackItem);
    //CURLOPT_WRITEDATA - pointer passed to the write callback
    curl_easy_setopt(curlVector, CURLOPT_WRITEDATA, (void *)&chunkReadBuffer);
    // Perform the request, res will get the return code
    res = curl_easy_perform(curlVector);
    // Check for errors
    if(res != CURLE_OK)
    {
      printf("curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    
    //cout << "vector<string> print on console" << endl;
    for(vector<string>::iterator it = vec.begin();
        it != vec.end();
        it++)
    {
      cout << *it << endl;
    }

    free(chunkReadBuffer.response);    
    
    // always cleanup
    curl_easy_cleanup(curlVector);
    
  }

  pthread_exit(0);

}

//-----------------------------------------------------------------------------

void *threadFunc(void *pvParam)
{

  const void *httpToESP = pvParam;
  pthread_t threadHttpESP;
/*
  uint16_t i;
  for(i = 0; i < SIZE_VEC; i++)
  {
		
		pthread_create(&threadHttpESP, NULL, threadOneReqFunc, httpToESP);
		
		pthread_join(threadHttpESP, NULL);
		
    usleep(500000);
			
  }
*/
	
  pthread_create(&threadHttpESP, NULL, threadHttpRequestFunc, httpToESP);
	
	pthread_join(threadHttpESP, NULL);

  pthread_exit(0);
}

//-----------------------------------------------------------------------------
/*
http://192.168.137.135:80/acc.sh?http://192.168.137.58:80/esp32?getParams=all


*/

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("usage %s http://192.168.xxx.yyy:80/esp32\n", argv[0]);
    exit(0);
  }

  void *threadData = argv[1];
  pthread_t thread;

  pthread_create(&thread, NULL, threadFunc, threadData);
  pthread_join(thread, NULL);

  return 0;
}
//-----------------------------------------------------------------------------
