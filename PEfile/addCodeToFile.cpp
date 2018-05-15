#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>

#include "initializeValue.h"
#include "getInfoOfFile.h"
#include "addCodeToFile.h"

using namespace std;

vector< pair<string, vector<string> > > importFunctions;
vector< pair<string, vector<int> > > offsetOfImportFunctions;
vector<int> offsetOfDlls;

bool canAddCode = true;

void initImportFunctions() {
    pair<string, vector<string> > dllFunctions;

    // sample of adding new dllFunctions
    dllFunctions.second = vector<string>();
    dllFunctions.first = "user32.dll";
    dllFunctions.second.push_back("MessageBoxA");
    importFunctions.push_back(dllFunctions);
}

// @ Put IID at the end of import section
// @ Add dll we need to IID
vector<int> addImportFunctions() {
    vector<int> outputData = fileData;

    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    int RVAImportTable = getValueOfField("importTableRVA", offsetOfPE);

    if (RVAImportTable == 0) {
        cout << "I can't add import Functions in this file........... because there are not a import section" << endl;
        canAddCode = false;
        return outputData;
    }

    offsetInDataDicrectory["RVAImportTable"] = RVAImportTable;
    getImportSectionData();
    int offsetImportTable = offsetInDataDicrectory["offsetImportTable"];

    int diffRVAandOffset = offsetImportTable - RVAImportTable;
    // RVA = [offset] - diffRVAandOffset + imageBase;

    printf("RVAImportTable: %.8X\t\t", RVAImportTable);
    printf("offsetImportTable: %.8X\t\t\n\n", offsetImportTable);

    int firstOffsetImportTable = offsetImportTable;
    int numberOfIID = 0;
    const int sizeOfIID = 4 * 5;

    while (true) {
        int RVAName1 = getValueOfField("name1", offsetImportTable);
        int RVAFirstThunk = getValueOfField("firstThunk", offsetImportTable);   
        if (RVAName1 == 0 && RVAFirstThunk == 0) {
            break;
        }
        numberOfIID++;
        offsetImportTable += sizeOfParts["IAT"] = sizeOfIID;
    }
    int IIDSpace = numberOfIID * sizeOfIID;
    int lastOffsetImportTable = firstOffsetImportTable + IIDSpace- 1;
    
    vector<int> dataOfIID = getRawValueFromFileData(firstOffsetImportTable, IIDSpace);
    printf("firstOffsetImportTable: %.8X\t\t", firstOffsetImportTable);
    printf("lastOffsetImportTable: %.8X\t\t\n\n", lastOffsetImportTable);

    // printf("\n%.8X\t\t", 0);
    // for(int i = 0; i < IIDSpace; i++) {
    //     printf("%02X ", dataOfIID[i]);
    //     if ((i + 1) % 16 == 0)
    //         printf("\n%.8X\t\t", i+1);
    // }

    int spaceForIID = (numberOfIID + importFunctions.size() + 1) * sizeOfIID; // +1 of end of IID
    int spaceForDllFuntion = 0;
    for (int i = 0; i < importFunctions.size(); i++) {
        string dllName = importFunctions[i].first;
        spaceForDllFuntion += dllName.size() + 1; // 1 byte of /00
        for (int j = 0; j < importFunctions[i].second.size(); j++) {
            string functionName = importFunctions[i].second[j];
            spaceForDllFuntion += functionName.size() + 2 + 1; // 2 byte of zero for Hint + 1 byte of /00
            spaceForDllFuntion += 4; // image Thunk Data contain RVA to this function
        }
        spaceForDllFuntion += 4; // endl of image Thunk Data
    }

    int spaceNeed = spaceForIID + spaceForDllFuntion;
    int lastOffsetImportSection = offsetImportTable + sizeOfParts["Section"] - 1;
    printf("lastOffsetImportSection: %.8X\n\n", lastOffsetImportSection);
    return outputData;
}

vector<int> addCodeToFile() {
    initImportFunctions();
    vector<int> outputData = addImportFunctions();
    if (!canAddCode) {
        return outputData;
    }

    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int addressOfEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    int fileAlignment = getValueOfField("fileAlignment", offsetOfPE);
    printf("\n\nPointerToEntryPoint: %.8X\n", addressOfEntryPoint);

    return outputData;
}