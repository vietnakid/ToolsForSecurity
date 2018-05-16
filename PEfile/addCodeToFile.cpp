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
vector<vector<int> > offsetOfImportFunctions;
vector<int> offsetOfDlls;
vector<int> offsetOfFirstThunk;
int diffRVAandOffset;
vector<vector<int> > RVAOfFunction;
map<string, vector<int> > assemblySyntax;

bool canAddCode = true;

void initImportFunctions() {
    pair<string, vector<string> > dllFunctions;

    // sample of adding new dllFunctions
    dllFunctions.second = vector<string>();
    dllFunctions.first = "user32.dll";
    dllFunctions.second.push_back("MessageBoxA");
    // dllFunctions.second.push_back("GetMessageA");
    importFunctions.push_back(dllFunctions);
}

int convertOffsetToRVA(int offset) {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    // int imageBase = getValueOfField("imageBase", offsetOfPE);
    int RVA = offset - diffRVAandOffset;
    return RVA;
}

vector<int> toRVAArray(int RVA) {
    vector<int> res(4);

    vector<int> base16OfRVA(8);
    for(int i = 7; i >= 0; i--) {
        base16OfRVA[i] = RVA % 16;
        RVA /= 16;
    }
    for(int i = 0; i < 4; i++) {
        int x = base16OfRVA[7-i*2-1]*16 + base16OfRVA[7-i*2];
        res[i] = x;
    }
    return res;
}

vector<int> genDllAndFunctionData(int baseOffset) {
    vector<int> res;
    for (int i = 0; i < importFunctions.size(); i++) {
        offsetOfDlls.push_back(res.size() + baseOffset);
        string dllName = importFunctions[i].first;
        for (int ii = 0; ii < dllName.size(); ii++) {
            res.push_back((int)dllName[ii]);
        }
        res.push_back(0);
        vector<int> offsetOfThisDllFunctions;
        
        for (int j = 0; j < importFunctions[i].second.size(); j++) {
            offsetOfThisDllFunctions.push_back(res.size() + baseOffset);
            res.push_back(0); res.push_back(0); // 2 byte of zero for Hint
            string functionName = importFunctions[i].second[j];
            for (int ii = 0; ii < functionName.size(); ii++) {
                res.push_back((int)functionName[ii]);
            }
            res.push_back(0);
        }
        offsetOfImportFunctions.push_back(offsetOfThisDllFunctions);
    }
    // printf("\n%.8X\t\t", 0);
    // for(int i = 0; i < res.size(); i++) {
    //     printf("%02X ", res[i]);
    //     if ((i + 1) % 16 == 0)
    //         printf("\n%.8X\t\t", i+1);
    // }
    return res;
}

int calSpaceForDllFuntion() {
    int spaceForDllFuntion = 0;
    for (int i = 0; i < importFunctions.size(); i++) {
        string dllName = importFunctions[i].first;
        spaceForDllFuntion += dllName.size() + 1; // 1 byte of /00
        for (int j = 0; j < importFunctions[i].second.size(); j++) {
            string functionName = importFunctions[i].second[j];
            spaceForDllFuntion += functionName.size() + 2 + 1; // 2 byte of zero for Hint + 1 byte of /00
            spaceForDllFuntion += 4; 
        }
    }
    return spaceForDllFuntion;
}

vector<int> gendataOfNewIID() {
    vector<int> res;
    vector<int> zero4(4, 0);
    for (int i = 0; i < offsetOfDlls.size(); i++) {
        vector<int> firstThunkRVA = toRVAArray(convertOffsetToRVA(offsetOfFirstThunk[i]));
        vector<int> name1RVA = toRVAArray(convertOffsetToRVA(offsetOfDlls[i]));

        res.insert(res.end(), firstThunkRVA.begin(), firstThunkRVA.end()); // Original First Thunk
        res.insert(res.end(), zero4.begin(), zero4.end()); // TimeDataStamp
        res.insert(res.end(), zero4.begin(), zero4.end()); // Forward Chain
        res.insert(res.end(), name1RVA.begin(), name1RVA.end()); // Name1
        res.insert(res.end(), firstThunkRVA.begin(), firstThunkRVA.end()); // FirstThunk
    }
    for (int i = 0; i < 5; i++) { // end of IID
        res.insert(res.end(), zero4.begin(), zero4.end());
    }
    return res;
}

int calSpaceForRVA() {
    int spaceForRVA = 0;
    for (int i = 0; i < importFunctions.size(); i++) {
        spaceForRVA += importFunctions[i].second.size() * 4; // image Thunks Data contain RVA to this function
        spaceForRVA += 4; // endl of image Thunk Data
    }
    return spaceForRVA;
}

vector<int> genRVAData(int baseOffset) {
    vector<int> res;
    int offset = 0;
    for (int i = 0; i < offsetOfDlls.size(); i++) {
        offsetOfFirstThunk.push_back(offset + baseOffset);
        vector<int> RVAOfThisDllFunction;
        for (int j = 0; j < offsetOfImportFunctions[i].size(); j++) {
            vector<int> RVA = toRVAArray(convertOffsetToRVA(offsetOfImportFunctions[i][j]));
            res.insert(res.end(), RVA.begin(), RVA.end());
            RVAOfThisDllFunction.push_back(convertOffsetToRVA(offset + baseOffset));
            offset += 4;
        }
        vector<int> zero4(4, 0);
        res.insert(res.end(), zero4.begin(), zero4.end());
        RVAOfFunction.push_back(RVAOfThisDllFunction);
    }

    return res;
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
    int sizeOfImportTable = offsetInDataDicrectory["sizeImportTable"];

    diffRVAandOffset = offsetImportTable - RVAImportTable;
    //@ Fomular RVA = [offset] - diffRVAandOffset + imageBase;

    // printf("RVAImportTable: %.8X\t\t", RVAImportTable);
    // printf("offsetImportTable: %.8X\t\t\n\n", offsetImportTable);

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

    int spaceForIID = (numberOfIID + importFunctions.size() + 1) * sizeOfIID; // +1 of end of IID
    int spaceForDllFuntion = calSpaceForDllFuntion();
    int spaceForRVA = calSpaceForRVA();
    int spaceNeed = spaceForIID + spaceForDllFuntion + spaceForRVA;

    int lastOffsetImportSection = firstOffsetImportTable + sizeOfImportTable - 1;
    // printf("firstOffsetImportTable: %.8X\t\t", firstOffsetImportTable);
    // printf("lastOffsetImportTable: %.8X\t\t\n\n", lastOffsetImportTable);
    // printf("sizeOfImportTable: %.8X\n\n", sizeOfImportTable);
    // printf("lastOffsetImportSection: %.8X\n\n", lastOffsetImportSection);
    // printf("spaceNeed: %.8X\n\n", spaceNeed);
    int startOffset = lastOffsetImportSection - spaceNeed + 1;
    startOffset = (startOffset / 16) * 16;
    // printf("startOffset: %.8X\n", startOffset);

    //@ Must keep this order of getting data
    vector<int> dataOfIID = getRawValueFromFileData(firstOffsetImportTable, IIDSpace);
    vector<int> dllAndFunctionData = genDllAndFunctionData(startOffset + spaceForIID + spaceForRVA);
    vector<int> RVAData = genRVAData(startOffset + spaceForIID);
    vector<int> dataOfNewIID = gendataOfNewIID();
    vector<int> dataOfNewImport = dataOfIID;
    dataOfNewImport.insert(dataOfNewImport.end(), dataOfNewIID.begin(), dataOfNewIID.end());
    dataOfNewImport.insert(dataOfNewImport.end(), RVAData.begin(), RVAData.end());
    dataOfNewImport.insert(dataOfNewImport.end(), dllAndFunctionData.begin(), dllAndFunctionData.end());
    // printf("\n%.8X\t\t", 0);
    // for(int i = 0; i < dataOfNewIID.size(); i++) {
    //     printf("%02X ", dataOfNewIID[i]);
    //     if ((i + 1) % 16 == 0)
    //         printf("\n%.8X\t\t", i+1);
    // }
    int j = 0;
    for (int i = startOffset; i <= lastOffsetImportSection; i++) {
        outputData[i] = dataOfNewImport[j];
        j++;
        if (j > dataOfNewImport.size()) {
            break;
        }
    }

    // Change RVA point to Import In Data Directory
    int offsetOfImport = offsetOfParts["importTableRVA"] + offsetOfPE;
    // printf("offsetOfParts[importTableRVA] : %.8X\n", offsetOfImport);
    int newRVAImport = startOffset - diffRVAandOffset;
    // printf("new RVA Import : %.8X\n", newRVAImport);
    vector<int> newRVAImportVector = toRVAArray(newRVAImport);
    for (int i = 0; i < 3; i++) {
        outputData[offsetOfImport + i] = newRVAImportVector[i];
    }

    // Change Size of Import Section
    int offsetImportSection = offsetInDataDicrectory["offsetImportSection"];
    int offsetVirtualSize = offsetInDataDicrectory["offsetImportSection"] + offsetOfParts["virtualSize"];
    int rawSizeImportSection = getValueOfField("sizeOfRawData", offsetImportSection);
    // printf("rawSizeImportSection: %.8X\n", rawSizeImportSection);
    // printf("offsetImportSection: %.8X\n", offsetImportSection);
    vector<int> newVirtualSize = toRVAArray(rawSizeImportSection);
    for (int i = 0; i < 3; i++) {
        outputData[offsetVirtualSize + i] = newVirtualSize[i];
    }

    // Change 
    return outputData;
}

int getOffsetOfCodeSection() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int originalEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    int sizeOfOneSection = sizeOfParts["Section"];
    int offsetOfSection = offsetOfParts["DataDirectories"] + offsetOfPE + sizeOfParts["DataDirectories"];
    for (int section = 0; section < numberOfSection; section++) {
        int firstOffsetOfSection = getValueOfField("virutalAddress", offsetOfSection);
        int lastOffsetOfSection = firstOffsetOfSection + getValueOfField("virtualSize", offsetOfSection);
        if (firstOffsetOfSection <= originalEntryPoint && originalEntryPoint <= lastOffsetOfSection) {
            return offsetOfSection;
        }
        offsetOfSection += sizeOfOneSection;
    }
    return 0;
}

void initAssemblySyntax() {
    assemblySyntax["CALL"] = vector<int>() = {255, 21}; // FF15
    assemblySyntax["MOV"] = vector<int>() = {184}; // B8
    assemblySyntax["JMP EAX"] = vector<int>() = {255, 224}; // FFE0
    assemblySyntax["PUSH-BYTE"] = vector<int>() = {106}; // 6A
    assemblySyntax["PUSH-PTR"] = vector<int>() = {104}; // 68
}

vector<int> getUserCode() {
    initAssemblySyntax();
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    vector<int> code, address;
    vector<int> zero = {0};

    // @ Push variable 4
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Push variable 3
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Push variable 2
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Push variable 1
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Call function MessageBoxA
    code.insert(code.end(), assemblySyntax["CALL"].begin(), assemblySyntax["CALL"].end());
    address = toRVAArray(RVAOfFunction[0][0] + imageBase);
    code.insert(code.end(), address.begin(), address.end());

    // @ MOV EAX, OriginalEntryPoint
    code.insert(code.end(), assemblySyntax["MOV"].begin(), assemblySyntax["MOV"].end());
    int originalEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    address = toRVAArray(originalEntryPoint + imageBase);
    code.insert(code.end(), address.begin(), address.end());

    // @ JPM EAX
    code.insert(code.end(), assemblySyntax["JMP EAX"].begin(), assemblySyntax["JMP EAX"].end());

    return code;
}

vector<int> addCode(vector<int> rawData) {
    vector<int> outputData = rawData;
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    int originalEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    
    int offsetCodeSection = getOffsetOfCodeSection();
    int virtualAddressCodeSection = getValueOfField("virutalAddress", offsetCodeSection);
    int rawOffsetCodeSection = getValueOfField("pointerToRawData", offsetCodeSection);
    int diffRVAandOffsetCodeSetion = rawOffsetCodeSection - virtualAddressCodeSection;

    // Reset VirtualSize of code section
    int offsetVirtualSize = offsetOfParts["virtualSize"] + offsetCodeSection;
    int rawSizeDataCodeSection = getValueOfField("sizeOfRawData", offsetCodeSection);
    vector<int> newVirtualSizeCodeSection = toRVAArray(rawSizeDataCodeSection);
    for (int i = 0; i < 3; i++) {
        outputData[offsetVirtualSize + i] = newVirtualSizeCodeSection[i];
    }

    // Add Code
    vector<int> userCode = getUserCode();
    int lastOffsetCodeSection = rawOffsetCodeSection + rawSizeDataCodeSection - 1;
    int startOffset = lastOffsetCodeSection - userCode.size() + 1;
    startOffset = (startOffset / 16) * 16;

    int j = 0;
    for (int i = startOffset; i <= lastOffsetCodeSection; i++) {
        outputData[i] = userCode[j];
        if (j > userCode.size()) {
            break;
        }
        j++;
    }

    // Set new Entry Point
    int offsetEntryPoint = offsetOfParts["addressOfEntryPoint"] + offsetOfPE;
    vector<int> newEntryRVA = toRVAArray(startOffset - diffRVAandOffsetCodeSetion);
    for (int i = 0; i < 3; i++) {
        outputData[offsetEntryPoint + i] = newEntryRVA[i];
    }

    return outputData;
}

vector<int> addCodeToFile() {
    initImportFunctions();
    vector<int> outputData = addImportFunctions();
    if (!canAddCode) {
        return outputData;
    }

    outputData = addCode(outputData);
    return outputData;
}