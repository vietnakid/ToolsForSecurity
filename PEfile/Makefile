all: clean compile
	./PEFile

compile: PEFile.o initializeValue.o getInfoOfFile.o addCodeToFile.o
	g++ -std=c++14 PEFile.o initializeValue.o getInfoOfFile.o addCodeToFile.o -o PEFile

PEFile.o: PEFile.cpp
	g++ -std=c++14 -c PEFile.cpp

initializeValue.o: initializeValue.cpp
	g++ -std=c++14 -c initializeValue.cpp

getInfoOfFile.o: getInfoOfFile.cpp
	g++ -std=c++14 -c getInfoOfFile.cpp

addCodeToFile.o: addCodeToFile.cpp
	g++ -std=c++14 -c addCodeToFile.cpp

clean:
	rm -rf *.o