#include <map>
#include <string>
#include <iostream>
#include <vector>

#include "getInfoOfFile.h"

using namespace std;

int getIntValueFromFileData(int offset, int size) {
    int pow16[8];
    pow16[0] = 1;
    for(int i = 1; i < 8; i++) {
        pow16[i] = pow16[i-1] * 16;
    }
    int res = 0;
    for(int i = 0; i < size; i++) {
        res += fileData[offset + i]*pow16[i*2];
    }
    return res;
}

string getRawValueFromFileData(int offset, int size) {
    string res = "";
    for(int i = 0; i < size; i++) {
        res += fileData[i + offset];
    }
    return res;
}

string getStringFromFileData(int offset) {
    string res = "";
    while (fileData[offset] != 0) {
        res += fileData[offset];
        offset++;
    }
    return res;
}

void showHexOfFile() {
    cout << "Hex dump :" << endl;
    printf("00000000\t\t");
    for(int i = 0; i < sizeOfFile; i++) {
        printf("%02X ", fileData[i]);
        if ((i + 1) % 16 == 0)
            printf("\n%.8X\t\t", i+1);
    }
}

int getValueOfField(string field, int baseOffset) {
    return getIntValueFromFileData(offsetOfParts[field] + baseOffset, sizeOfParts[field]);
}

void showInfoOfPEHeader() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    printf("Offset of PE: %.8X \n\n", offsetOfPE);
    printf("PointerToEntryPoint: %.8X\n", getValueOfField("addressOfEntryPoint", offsetOfPE));
    printf("Characteristics: %.8X\n", getValueOfField("characteristicsOfSectionTable", offsetOfPE));
    printf("ImageBase: %.8X\n", getValueOfField("imageBase", offsetOfPE));
    printf("FileAlignment: %.8X\n", getValueOfField("fileAlignment", offsetOfPE));
    printf("SizeOfImage: %.8X\n", getValueOfField("sizeOfImage", offsetOfPE));
    printf("\n\n");
}

void showInfoOfSection() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F

    string valueToGet;
    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    printf("There are %d sections..\n\n", numberOfSection);

    int sizeOfOneSection = sizeOfParts["Section"];

    valueToGet = "DataDirectories";
    int offsetOfSection = offsetOfParts[valueToGet] + offsetOfPE + sizeOfParts[valueToGet];

    for (int section = 0; section < numberOfSection; section++) {
        printf("Data of section %d: \n", section + 1);
        printf("Offset of section: %.8X\n", offsetOfSection);

        valueToGet = "name";
        string nameOfSection = getRawValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]);
        cout << "Name : " << nameOfSection << '\n';

        printf("Characteristics: %.8X\n", getValueOfField("characteristicsOfSectionTable", offsetOfSection));

        printf("RawAddress: %.8X\n", getValueOfField("pointerToRawData", offsetOfSection));

        printf("RawSize: %.8X\n", getValueOfField("sizeOfRawData", offsetOfSection));

        printf("VirtualAddress: %.8X\n", getValueOfField("virutalAddress", offsetOfSection));

        printf("VirtualSize: %.8X\n", getValueOfField("virtualSize", offsetOfSection));

        printf("\n\n");

        offsetOfSection += sizeOfOneSection;
    }
}

void getImportSectionData() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F

    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    int sizeOfOneSection = sizeOfParts["Section"];
    int offsetOfSection = offsetOfParts["DataDirectories"] + offsetOfPE + sizeOfParts["DataDirectories"];
    for (int section = 0; section < numberOfSection; section++) {
        int firstOffsetOfSection = getValueOfField("virutalAddress", offsetOfSection);
        int lastOffsetOfSection = firstOffsetOfSection + getValueOfField("virtualSize", offsetOfSection);
        if (firstOffsetOfSection <= offsetInDataDicrectory["RVAImportTable"] && offsetInDataDicrectory["RVAImportTable"] <= lastOffsetOfSection) {
            offsetInDataDicrectory["offsetImportTable"] = offsetInDataDicrectory["RVAImportTable"] - (firstOffsetOfSection - getValueOfField("pointerToRawData", offsetOfSection));
        }
        offsetOfSection += sizeOfOneSection;
    }
}

void getExportSectionData() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F

    string valueToGet;
    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    int sizeOfOneSection = sizeOfParts["Section"];
    valueToGet = "DataDirectories";
    int offsetOfSection = offsetOfParts[valueToGet] + offsetOfPE + sizeOfParts[valueToGet];
    for (int section = 0; section < numberOfSection; section++) {
        int firstOffsetOfSection = getValueOfField("virutalAddress", offsetOfSection);
        int lastOffsetOfSection = firstOffsetOfSection + getValueOfField("virtualSize", offsetOfSection);
        if (firstOffsetOfSection <= offsetInDataDicrectory["RVAExportTable"] && offsetInDataDicrectory["RVAExportTable"] <= lastOffsetOfSection) {
            offsetInDataDicrectory["offsetExportTable"] = offsetInDataDicrectory["RVAExportTable"] - (firstOffsetOfSection - getValueOfField("pointerToRawData", offsetOfSection));
        }
        offsetOfSection += sizeOfOneSection;
    }
}

void showImports() {
    cout << "\nImport:" << "\n";
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int RVAImportTable = getValueOfField("importTableRVA", offsetOfPE);

    if (RVAImportTable == 0) {
        cout << "There are no Import Address Table" << endl;
    }

    offsetInDataDicrectory["RVAImportTable"] = RVAImportTable;
    getImportSectionData();
    int offsetImportTable = offsetInDataDicrectory["offsetImportTable"];

    printf("RVAImportTable: %.8X\t\t", RVAImportTable);
    printf("offsetImportTable: %.8X\t\t\n\n", offsetImportTable);

    int diffRVAandOffset = offsetImportTable - RVAImportTable;

    while (true) {
        int RVAName1 = getValueOfField("name1", offsetImportTable);
        int RVAFirstThunk = getValueOfField("firstThunk", offsetImportTable);   
        if (RVAName1 == 0 && RVAFirstThunk == 0) {
            return;
        }
        int offsetName1 = RVAName1 + diffRVAandOffset;
        int offsetFirstThunk = RVAFirstThunk + diffRVAandOffset;
        cout << "\n\n" << getStringFromFileData(offsetName1) <<  "\t";

        int RVAOriginalFirstThunk = getValueOfField("originalFirstThunk", offsetImportTable); 
        int offsetOfOriginalFirstThunk = RVAOriginalFirstThunk + diffRVAandOffset;
        printf("RVAOriginalFirstThunk: %.8X\t\t", RVAOriginalFirstThunk);
        // printf("offsetOfOriginalFirstThunk: %.8X\t\t", offsetOfOriginalFirstThunk);
        printf("RVAFirstThunk: %.8X\t\t", RVAFirstThunk);
        printf("offsetFirstThunk: %.8X\n", offsetFirstThunk);

        // Thunk data
        // case OriginalFirstThunk is null
        if (RVAOriginalFirstThunk == 0) {
            while (true) {
                int RVAImportByName = getValueOfField("ThunkData", offsetFirstThunk);
                int offsetImportByName = RVAImportByName + diffRVAandOffset;

                if (RVAImportByName == 0) {
                    break;
                }

                int offsetHint = offsetImportByName;
                int hint = getValueOfField("hint", offsetHint);
                int thunkValue = getValueOfField("ThunkData", offsetFirstThunk);
                offsetImportByName += offsetOfParts["name1Thunk"];
                cout << "\t" << getStringFromFileData(offsetImportByName) << "\t";
                printf("\tHint: %.4X\tThunk RVA: %.8X\tThunk Offset: %.8X\tThunk Data: %.8X\n", hint, RVAFirstThunk, offsetFirstThunk, thunkValue);

                offsetFirstThunk += sizeOfParts["ThunkData"];
            }
        } else {
            while (true) {
                // Original First Thunk
                int RVAImgaeThunkData = getValueOfField("ThunkData", offsetOfOriginalFirstThunk);
                int offsetImageThunkData = RVAImgaeThunkData + diffRVAandOffset;

                if (RVAImgaeThunkData == 0) {
                    break;
                }

                int offsetHint = offsetImageThunkData;
                int hint = getValueOfField("hint", offsetHint);
                int thunkValue = getValueOfField("ThunkData", offsetFirstThunk);
                int offsetName = offsetImageThunkData + offsetOfParts["name1Thunk"];
                cout << "\t" << getStringFromFileData(offsetName) << "\t";
                printf("\tHint: %.4X\tThunk RVA: %.8X\tThunk Offset: %.8X\tThunk Data: %.8X\n", hint, RVAFirstThunk, offsetFirstThunk, thunkValue);

                RVAFirstThunk += sizeOfParts["ThunkData"];
                offsetFirstThunk += sizeOfParts["ThunkData"];
                offsetOfOriginalFirstThunk += sizeOfParts["ThunkData"];
            }
        }

        offsetImportTable += sizeOfParts["IAT"] = 4 * 5;
    }
    
}

void showExports() {
    cout << "\nExports:" << "\n";
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int RVAExportTable = getValueOfField("exportTableRVA", offsetOfPE);

    if (RVAExportTable == 0) {
        cout << "There are no Export" << endl;
        return;
    }

    offsetInDataDicrectory["RVAExportTable"] = RVAExportTable;
    getExportSectionData();
    int offsetExportTable = offsetInDataDicrectory["offsetExportTable"];
    int diffRVAandOffset = offsetExportTable - RVAExportTable;

    int numberOfFunctions = getValueOfField("numberOfFuntions", offsetExportTable);
    cout << "There are " << numberOfFunctions << " export functions: " << endl;

    int RVAAddressOfFuntions = getValueOfField("addressOfFuntions", offsetExportTable);
    int offsetAddressOfFuntions = RVAAddressOfFuntions + diffRVAandOffset;

    int numberOfNames = getValueOfField("numberOfNames", offsetExportTable);
    cout << "\tThere are " << numberOfNames << " named functions: " << endl;

    int RVANameOfFunctions = getValueOfField("addressOfNames", offsetExportTable);
    int offsetNameOfFunctions = RVANameOfFunctions + diffRVAandOffset;
    int RVAOfNamesOrdinals = getValueOfField("addressOfNamesOrdinals", offsetExportTable);
    int offsetOfNameOrdinals = RVAOfNamesOrdinals + diffRVAandOffset;
    for (int indexOfNames = 0; indexOfNames < numberOfNames; indexOfNames++) {
        int RVAOfName = getValueOfField("nameFunction", offsetNameOfFunctions + indexOfNames * sizeOfParts["nameFunction"]);
        if (RVAOfName == 0) {
            break;
        }
        int offsetOfName = RVAOfName + diffRVAandOffset;
        int ordinal = getValueOfField("ordinal", offsetOfNameOrdinals + indexOfNames * sizeOfParts["ordinal"]);
        int RVAAddressOfFunction = getValueOfField("addressOfFunction", offsetAddressOfFuntions + indexOfNames * sizeOfParts["addressOfFunction"]);
        int offsetAddressOfFunction = RVAAddressOfFunction + diffRVAandOffset;
        printf("\t\tOrdinal: %.4X\tRVA: %.8X\tOffset: %.8X\tName: ", ordinal, RVAAddressOfFunction, offsetAddressOfFunction);
        cout << getStringFromFileData(offsetOfName) << endl;
    }

    cout << "\n\tThere are " << numberOfFunctions - numberOfNames << " ordinal functions: " << endl;
    for (int indexOfOrdinalFunction = numberOfNames; indexOfOrdinalFunction < numberOfFunctions; indexOfOrdinalFunction++) {
        int ordinal = getValueOfField("ordinal", offsetOfNameOrdinals + indexOfOrdinalFunction * sizeOfParts["ordinal"]);
        int RVAAddressOfFunction = getValueOfField("addressOfFunction", offsetAddressOfFuntions + indexOfOrdinalFunction * sizeOfParts["addressOfFunction"]);
        int offsetAddressOfFunction = RVAAddressOfFunction + diffRVAandOffset;
        printf("\t\tOrdinal: %.4X\tRVA: %.8X\tOffset: %.8X\n", ordinal, RVAAddressOfFunction, offsetAddressOfFunction);
    }
}

void showInfoOfFile() {
    // showInfoOfPEHeader();
    // showInfoOfSection();
    showImports();
    showExports();
    // showHexOfFile();
}