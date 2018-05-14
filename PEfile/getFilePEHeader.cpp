#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "initializeValue.h"
#include "getInfoOfFile.h"

using namespace std;

string filePath = "";
streampos sizeOfFile;
char * rawData;
map<string, int> sizeOfParts;
map<string, int> offsetOfParts;
map<string, int> offsetInDataDicrectory;
vector<int> fileData;

void constructIntDataFromRawData() {
    for(int i = 0; i < sizeOfFile; i++) {
        int hex = rawData[i];
        if (hex < 0) {
            hex += 256;
        }
        fileData[i] = hex;
    }
}

bool isPEFile() {
    if (fileData[0] != 'M' || fileData[1] != 'Z') {
        return false;
    }
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    if (fileData[offsetOfPE] != 'P' || fileData[offsetOfPE + 1] != 'E') {
        return false;
    }
    return true;
}

int main(int argc,  char** argv) {
    // filePath = argv[1];
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/BASECALC/BASECALC.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/FAKE.EXE";
    filePath = "/Users/macbook/Desktop/SharedWithWindows/twain_32.dll";
    init();
    ifstream file (filePath.c_str(), ios::in|ios::binary|ios::ate);
    cout << filePath.c_str() << endl;
    if (file.is_open())
    {
        sizeOfFile = file.tellg();
        fileData = vector<int>(sizeOfFile);
        rawData = new char [sizeOfFile];
        file.seekg (0, ios::beg);
        file.read (rawData, sizeOfFile);
        file.close();
        constructIntDataFromRawData();
        if (isPEFile()) {
            // showInfoOfPEHeader();
            // showInfoOfSection();
            showImports();
            showExports();
            // showHexOfFile();
        } else {
            printf("This a not a PE file\n");
        }
        delete[] rawData;
    }
    else cout << "Unable to open file\n";
    return 0;
}
