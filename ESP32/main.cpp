#include <ArduinoJson.h>

/*
 *  ESP8266 JSON Encode Server
 *  -Manoj R. Thkuar
 *  https://circuits4you.com
 */
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <iostream>
#include <ESPmDNS.h>
#include <stdint-gcc.h>

bool isConfigIP = false;

// const char* wifiName = "GUEST@VLSU";
// const char* wifiPass = "2718281828";

// const char* wifiName = "NPKOSV";
// const char* wifiPass = "E?9+7e%q";

const char* wifiName = "OSV_MERCUSYS";
const char* wifiPass = "40373911";

String host = "http://";

uint16_t countRequest = 0;

String serverName = "";
int serverPort = 80;
  
WebServer server(serverPort);  //Define server object
StaticJsonDocument<256> jsonBuffer;

void generateJsonBufAntTX(String jsonKey, int jsonVal)
{
  String webPage;

  jsonBuffer[jsonKey] = jsonVal; //Put Sensor value

  serializeJson(jsonBuffer, webPage);  //Store JSON in String variable

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "1000");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS, DELETE, PUT");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, Content-Type, Accept, Authorization, X-Request-With, X-CLIENT-ID, X-CLIENT-SECRET");
  server.sendHeader("Access-Control-Allow-Credentials:", "true");

  server.send(200, "text/html", webPage);

  Serial.println(webPage);
}

/*--===================================================================================================--*/
/*--===================================================================================================--*/
/*--===================================================================================================--*/

#define GLOBAL_ROW  6
#define GLOBAL_COLUMN  20

//format message
// for led
//{"isPressed":"pressed(released)","butWhatIs":"led_red(green, blue, yellow)","counter":238}
// for input
//{"isPressed":"pressed(released)","butWhatIs":"input_0(..3)","counter":}
// for slider
//{"isPressed":"released","butWhatIs":"slider","counter":0..100}

char ArrayJSON[GLOBAL_ROW][GLOBAL_COLUMN];


//-----------------------------------------------------------------------------

void fillArray(char *twoDimensionalArr, const unsigned row, const unsigned column, const char cFill)
{
  for(unsigned i = 0; i < row; i++)
  {
    for(unsigned j = 0; j < column; j++)
    {
      *(twoDimensionalArr + (i * column) + j) = cFill;
    }
  }
}

//-----------------------------------------------------------------------------

unsigned divideOnTocken(const char* ptrSumbol)
{
  unsigned countWord = 0;
  unsigned lenWordCurrent = 0;
  unsigned lenWordSave = 0;
  unsigned countArray = 0;
  unsigned cntChar = 0;

  bool isBegin = false;
  bool isEnd = false;
  bool isWord = false;

  //const char* pBegin = ptrSumbol;
  const char* ptrToArr = nullptr;

  while(*ptrSumbol)
  {
    if( ( ((*ptrSumbol) >= '0') && ((*ptrSumbol) <= '9') ) ||
        ( ((*ptrSumbol) >= 'A') && ((*ptrSumbol) <= 'Z') ) ||
        ( ((*ptrSumbol) >= 'a') && ((*ptrSumbol) <= 'z') ) ||
          ((*ptrSumbol) == '_') ||
          ((*ptrSumbol) == '-')
       )
    {
      // 0..9 A..Z a..z - _
      if(isBegin)
      {
        isWord = true;
        isBegin = false;
      }
      lenWordCurrent++;
    }
    else
    {
      // { } [ ] "
      if(lenWordCurrent)
      {
        isEnd = true;
        lenWordSave = lenWordCurrent;
      }
      isBegin = true;
      lenWordCurrent = 0;
    }
    if(isWord)
    {
      isWord = false;
      ptrToArr = ptrSumbol;
      countWord++;
    }
    if(isEnd)
    {
      isEnd = false;
      cntChar = 0;
      while(cntChar < lenWordSave)
      {
        ArrayJSON[countArray][cntChar] = *(ptrToArr + cntChar);
        cntChar++;
      }
      countArray++;
    }
    ptrSumbol++;
  }
  return countWord;
}

//-----------------------------------------------------------------------------

//#define PRINT_RX_FULL_MSG

unsigned parsingMessageFromClient(void)
{
  unsigned retLen = 0;

  retLen = server.args();

#if defined(PRINT_RX_FULL_MSG)
  String message = "Number of args received = ";
#endif  //PRINT_RX_FULL_MSG
#if defined(PRINT_RX_FULL_MSG)
  message += server.args();      // получить количество параметров
  message += "\n";               // переход на новую строку
#endif  //PRINT_RX_FULL_MSG

  if(retLen == 0)
  {
#if defined(PRINT_RX_FULL_MSG)
  Serial.println(message);
#endif  //PRINT_RX_FULL_MSG
    return 0;
  }

  String jsonmessage;

  for (int i = 0; i < server.args(); i++)
  {
#if defined(PRINT_RX_FULL_MSG)
    message += "Arg Nº" + (String)i + " –> "; // добавить текущее значение счетчика
    message += server.argName(i) + ": ";      // получить имя параметра
    message += server.arg(i) + "\n";          // получить значение параметра
#endif  //PRINT_RX_FULL_MSG
    jsonmessage = server.arg(i);
  }

#if defined(PRINT_RX_FULL_MSG)
  Serial.println(message);
#endif  //PRINT_RX_FULL_MSG

  //Serial.println("rx String message : " + jsonmessage);

  retLen = divideOnTocken(jsonmessage.c_str());

  return retLen;

}

/*--===================================================================================================--*/
/*--===================================================================================================--*/
/*--===================================================================================================--*/

#define AMOUNT_HANDLER  4

//-----------------------------------------------------------------------------

const char *slider_strings = "slider";
const static unsigned length_slider = 6;

void createJsonMessageSlider(void)
{
  static unsigned slider = 0;
//format message
// for slider
//{"isPressed":"released","butWhatIs":"slider","value":0..100}

  if(!memcmp(ArrayJSON[3], slider_strings, length_slider))
  {
    jsonBuffer[slider_strings] = slider;
    
    jsonBuffer["slider"] = "working";
    
    jsonBuffer["value"] = atoi(ArrayJSON[5]);
  }

}

void handlerSlider(void)
{ 

  String response;
  unsigned length;
  length = parsingMessageFromClient();

  if(length == 0)
  {
    server.send(200, "application/text", "{ \"uname\" : \"Linux nanopiair 4.19.13-sunxi #5.69 SMP Wed Jan 9 16:26:48 CET 2019 armv7l GNU/Linux\" }");
    return;
  }

  jsonBuffer.clear();

  createJsonMessageSlider();

  serializeJson(jsonBuffer, response);

  server.send(200, "application/json", response);

  //Serial.print("tx JSON : ");
  //Serial.println(response);

  for(unsigned i = 0; i < GLOBAL_ROW;)
  {
    Serial.print(ArrayJSON[i++]);
    Serial.print(" : ");
    Serial.println(ArrayJSON[i++]);
  }

  fillArray(&ArrayJSON[0][0], GLOBAL_ROW, GLOBAL_COLUMN, (char)0);
}

//-----------------------------------------------------------------------------

const char *inputs_strings[] = { "input_0" , "input_1" ,"input_2" , "input_3" };
const static unsigned length_in[AMOUNT_HANDLER] = {7, 7, 7, 7};

unsigned forInput = 0;

void createJsonMessageInputs(void)
{
  static unsigned input[AMOUNT_HANDLER] = {0, 0, 0, 0};
  
//format message
// for input
//{"isPressed":"pressed(released)","butWhatIs":"input_0(..3)","counter":0(, 1, 2, ..)}

  for(unsigned i = 0; i < AMOUNT_HANDLER; i++)
  {
    if(!memcmp(ArrayJSON[3], inputs_strings[i], length_in[i]))
    {
      if(!memcmp(ArrayJSON[1], "pressed", 7))
      {
        //forInput ^= 0x01;
        forInput = static_cast<unsigned long>(random(0, 10));

        if(forInput & 0x01)
        {
          input[i] = 1;
        }
        else
        {
          input[i] = 0;
        }
      }
      jsonBuffer[inputs_strings[i]] = input[i];
      if(input[i])
      {
        jsonBuffer["button"] = "3.3V";
      }
      else
      {
        jsonBuffer["button"] = "0.0V";
      }
      jsonBuffer["random"] = random(-1000, 1000); //Put Sensor value
      
      break;
    }    
  }
}

//-----------------------------------------------------------------------------

const char *led_strings[] = { "led_red" , "led_green" ,"led_blue" , "led_yellow" };
const static unsigned length_led[AMOUNT_HANDLER] = {7, 9, 8, 10};

void createJsonMessageLeds(void)
{
  static unsigned led[AMOUNT_HANDLER] = {0, 0, 0, 0};
  
//format message
// for led
//{"isPressed":"pressed(released)","butWhatIs":"led_red(green, blue, yellow)","counter":238}

  //ArrayJSON[0] = isPressed
  //ArrayJSON[1] = pressed(released)
  //ArrayJSON[2] = butWhatIs
  //ArrayJSON[3] = led_red, led_green, led_blue, led_yellow
  //ArrayJSON[4] = counter
  //ArrayJSON[5] = 0, 1, 2, ..

  for(unsigned i = 0; i < AMOUNT_HANDLER; i++)
  {
    if(!memcmp(ArrayJSON[3], led_strings[i], length_led[i]))
    {
      if(!memcmp(ArrayJSON[1], "pressed", 7))
      {
        led[i] = 1;
      }
      if(!memcmp(ArrayJSON[1], "released", 8))
      {
        led[i] = 0;
      }
      jsonBuffer[led_strings[i]] = led[i];
      if(led[i])
      {
        jsonBuffer["lightbulb"] = "3.3V";
      }
      else
      {
        jsonBuffer["lightbulb"] = "0.0V";
      }
      jsonBuffer["random"] = random(-1000, 1000); //Put Sensor value
      
      break;
    }    
  }

}

//-----------------------------------------------------------------------------

//Handles http request 

void handlerLeds()
{
  String response;
  unsigned length;
  length = parsingMessageFromClient();

  if(length == 0)
  {
    server.send(200, "application/text", "{ \"uname\" : \"Linux nanopiair 4.19.13-sunxi #5.69 SMP Wed Jan 9 16:26:48 CET 2019 armv7l GNU/Linux\" }");
    return;
  }

  jsonBuffer.clear();

  createJsonMessageLeds();

  serializeJson(jsonBuffer, response);

  server.send(200, "application/json", response);

  //Serial.print("tx JSON : ");
  //Serial.println(response);

  for(unsigned i = 0; i < GLOBAL_ROW;)
  {
    Serial.print(ArrayJSON[i++]);
    Serial.print(" : ");
    Serial.println(ArrayJSON[i++]);
  }


  fillArray(&ArrayJSON[0][0], GLOBAL_ROW, GLOBAL_COLUMN, (char)0);
}

//-----------------------------------------------------------------------------

void handlerInputs()
{
  String response;
  unsigned length;
  length = parsingMessageFromClient();

  if(length == 0)
  {
    server.send(200, "application/text", "{ \"uname\" : \"Linux nanopiair 4.19.13-sunxi #5.69 SMP Wed Jan 9 16:26:48 CET 2019 armv7l GNU/Linux\" }");
    return;
  }

  jsonBuffer.clear();

  createJsonMessageInputs();

  serializeJson(jsonBuffer, response);

  server.send(200, "application/json", response);

  //Serial.print("tx JSON : ");
  //Serial.println(response);

  for(unsigned i = 0; i < GLOBAL_ROW;)
  {
    Serial.print(ArrayJSON[i++]);
    Serial.print(" : ");
    Serial.println(ArrayJSON[i++]);
  }
  
  fillArray(&ArrayJSON[0][0], GLOBAL_ROW, GLOBAL_COLUMN, (char)0);
}

//-----------------------------------------------------------------------------

void handlerESP32()
{
  static unsigned counterEnter = 0;
  String response;
  unsigned length = 0;
  uint16_t randVal;

  length = server.args();

  String message = " ( " + static_cast<String>(counterEnter++) +  " ) Number of args received = ";

  message += static_cast<String>(length);      // получить количество параметров
  message += "\n";               // переход на новую строку

  for (int i = 0; i < length; i++)
  {
    message += "Arg " + static_cast<String>(i) + " : "; // добавить текущее значение счетчика
    message += server.argName(i) + " = ";      // получить имя параметра
    message += server.arg(i) + "\n";          // получить значение параметра
  }

  Serial.println(message);


  if((length == 1) && (server.argName(0).equals("getParams")) && ((server.arg(0).equals("all"))))
  {
    jsonBuffer.clear();
    randVal = (uint16_t) random(-1000, 1000);

    jsonBuffer["getParam"] = "ESP32U";

    jsonBuffer["in0"] = randVal & (1 << 0)?1:0;
    jsonBuffer["in1"] = randVal & (1 << 1)?1:0;
    jsonBuffer["in2"] = randVal & (1 << 2)?1:0;
    jsonBuffer["in3"] = randVal & (1 << 3)?1:0;

    jsonBuffer["red"]    = randVal & (1 << 4)?1:0;
    jsonBuffer["green"]  = randVal & (1 << 5)?1:0;
    jsonBuffer["blue"]   = randVal & (1 << 6)?1:0;
    jsonBuffer["yellow"] = randVal & (1 << 7)?1:0;
    jsonBuffer["random"] = randVal;

    serializeJson(jsonBuffer, response);

    server.send(200, "application/json", response);

  }
  else
  {
    if(!length)
    {
      int r = random(-1000, 1000);
      server.send(200, "application/text", "{ \"uname(" + static_cast<String>(r) + ")\" : \"Linux nanopiair 4.19.13-sunxi #5.69 SMP Wed Jan 9 16:26:48 CET 2019 armv7l GNU/Linux\" }");
    }

    server.send(200, "application/text", message);
  }

}

//-----------------------------------------------------------------------------

void handleNotFound(void)
{
    if (server.method() == HTTP_OPTIONS)
    {
        // server.sendHeader("Access-Control-Allow-Origin", "*");
        // server.sendHeader("Access-Control-Max-Age", "10000");
        // server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        // server.sendHeader("Access-Control-Allow-Headers", "*");
        server.send(204);
    }
    else
    {
        server.send(404, "text/plain", "");
    }
}

// Define routing
void restServerRouting(void)
{
  server.on("/", []()
  {
    int r = random(-1000, 1000);
    String s = "Hello From ESP32 (rand = " + static_cast<String>(r) + " )";
    Serial.println(s);
    server.send(200, "text/html", s);
  });
}

// the setup function runs once when you press reset or power the board
void setup(void)
{

  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  Serial.print("Connecting");

  WiFi.begin(wifiName, wifiPass);   //Connect to WiFi

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  serverName = WiFi.localIP().toString();
  Serial.println("IP address: ");
  Serial.println(serverName);   //You can get IP address assigned to ESP

  host += serverName;

  server.on("/leds", handlerLeds);
  server.on("/inputs", handlerInputs);
  server.on("/slider", handlerSlider);
  server.on("/esp32", handlerESP32);
  server.onNotFound(handleNotFound);
  server.enableCORS(true);

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp32"))
  {
    Serial.println("mDNS responder started");
  }
  else
  {
    Serial.println("mDNS responder NOT started");
  }
  // Set server routing
  restServerRouting();

  server.begin(); //Start web server
  Serial.println("HTTP server started");

}

void loop(void)
{
  //Handle Clinet requests
  server.handleClient();

}