.PHONY: all

CXX = g++ -g -std=c++11
LDFLAGS = -lmosquittopp -lmosquitto -levent

all: server wj-mosquitto wj-mosquitto2

server: server.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

wj-mosquitto: client.o MainServer.o TMqttWrapper.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

wj-mosquitto2: client2.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

clean:
	rm -rf server wj-mosquitto wj-mosquitto2 *.o core
