#include <map>
#include <vector>
#include <string>
#include <utility>

#include "initializeValue.h"
#include "getInfoOfFile.h"
#include "addCodeToFile.h"

using namespace std;

vector< pair<string, vector<string> > > importFunctions;

void initImportFunctions() {
    pair<string, vector<string> > dllFunctions;

    // sample of adding new dllFunctions
    dllFunctions.second = vector<string>();
    dllFunctions.first = "user32.dll";
    dllFunctions.second.push_back("MessageBoxA");
    importFunctions.push_back(dllFunctions);
}

vector<int> addImportFunctions() {
    vector<int> outputData = fileData;
    return outputData;
}

vector<int> addCodeToFile() {
    initImportFunctions();
    vector<int> outputData = addImportFunctions();

    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int addressOfEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    int fileAlignment = getValueOfField("fileAlignment", offsetOfPE);
    printf("PointerToEntryPoint: %.8X\n", addressOfEntryPoint);

    return outputData;
}