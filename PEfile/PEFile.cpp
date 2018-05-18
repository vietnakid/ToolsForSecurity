#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "initializeValue.h"
#include "getInfoOfFile.h"
#include "addCodeToFile.h"
#include "getValueFromFile.h"

using namespace std;

string filePath = "";
map<string, int> sizeOfParts;
map<string, int> offsetOfParts;
map<string, int> offsetInDataDicrectory;

vector<int> fileData;
streampos sizeOfFile;

vector<int> constructIntDataFromRawData(char * rawData) {
    vector<int> fileData(sizeOfFile);
    for(int i = 0; i < sizeOfFile; i++) {
        int hex = rawData[i];
        if (hex < 0) {
            hex += 256;
        }
        fileData[i] = hex;
    }
    return fileData;
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

void saveFileWithNewData(vector<int> outputData, char * savePath) {
    ofstream outputFile (savePath, ios::out | ios::binary);
    for (int i = 0; i < sizeOfFile; i++) {
        outputFile.put((char) (outputData[i]));
    }
    cout << "\nNew file has been saved with filePath: \n" << savePath << endl;
}

int main(int argc,  char** argv) {
    // filePath = argv[1];
    
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/BASECALC/BASECALC.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/FAKE.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/c42bdc8fa6c1900c91539c647d8227c1.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/twain_32.dll";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/PEFile.exe";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/test.exe";
    initPEparams();
    ifstream file (filePath.c_str(), ios::in|ios::binary|ios::ate);
    cout << filePath << endl;
    if (file.is_open())
    {
        
        sizeOfFile = file.tellg(); // get Size of Data
        char * rawData = new char [sizeOfFile];
        file.seekg (0, ios::beg);
        file.read (rawData, sizeOfFile); // get RawData from file
        file.close();
        fileData = constructIntDataFromRawData(rawData); // From RawData to Integer data

        if (isPEFile()) {
            // showInfoOfFile();

            vector<int> outputData = addCodeToFile();
            string savePath = "/Users/macbook/Desktop/SharedWithWindows/test.exe";
            saveFileWithNewData(outputData,(char*) savePath.c_str());
        } else {
            printf("This a not a PE file\n");
        }
        delete[] rawData;
    }
    else cout << "Unable to open file\n";
    return 0;
}
