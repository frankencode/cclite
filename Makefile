testEvents: *.cc tt/*
	g++ -fPIE -Wall -pthread -pipe -std=c++11 -I. *.cc -o $@

clean:
	rm -rf testEvents
