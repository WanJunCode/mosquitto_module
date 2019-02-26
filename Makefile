.PHONY: all

all:
	g++ -std=c++11 -g -o main *.cpp -lmosquittopp -lmosquitto

clean:
	rm -rf main core