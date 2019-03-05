.PHONY: all

CXX = g++ -g -std=c++11
LDFLAGS = -lmosquittopp -lmosquitto -levent

all: server client

server: server.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

client: client.o MainServer.o TMqttWrapper.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

clean:
	rm -rf server client *.o core
