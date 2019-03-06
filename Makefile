.PHONY: all

CXX = g++ -g -std=c++11
LDFLAGS = -lmosquittopp -lmosquitto -levent

all: server client client2

server: server.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

client: client.o MainServer.o TMqttWrapper.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

client2: client2.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

clean:
	rm -rf server client client2 *.o core
