OBJ:=main.o
GCC:=g++
FLAGS:=-c -g -Wall -O0
LDLIBS:= -lpthread -lm -lfcgi++ -lfcgi
INCCURL:= -I../work_net/curl_lib/include
LIBCURL:= -L../work_net/curl_lib/lib/.libs -lcurl
INC:=./inc/
SRC:=./src/

all: top
top: $(OBJ)
	$(GCC) $(OBJ) $(INCCURL) $(LIBCURL) $(LDLIBS) -o top
	rm -rf *.o
main.o: $(SRC)main.cpp
	$(GCC) -I$(INC) $(INCCURL) $(LIBCURL) $(FLAGS) $(SRC)main.cpp
clean:
	find . -type f | xargs touch
	rm -rf *.o *scv top
