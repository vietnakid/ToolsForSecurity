#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

string filePath = "";
streampos size;
char * rawData;
map<string, int> sizeOfParts;
map<string, int> offsetOfParts;
vector<int> fileData;

void initSizeOfParts() {
    // COFF Header
    sizeOfParts["signature"] = 4;
    sizeOfParts["machine"] = 2;
    sizeOfParts["numberOfSections"] = 2;
    sizeOfParts["timeDateStamp"] = 4;
    sizeOfParts["pointerToSymbolTable"] = 4;
    sizeOfParts["numberOfSymbleTable"] = 4;
    sizeOfParts["sizeOfOptionalHeader"] = 2;
    sizeOfParts["characteristics"] = 2;

    // Standard COFF Fields
    sizeOfParts["magic"] = 2;
    sizeOfParts["majorLinkerVersion"] = 1;
    sizeOfParts["minorLinkerVersion"] = 1;
    sizeOfParts["sizeOfCode"] = 4;
    sizeOfParts["sizeOfInitializedData"] = 4;
    sizeOfParts["sizeOfUninitializedData"] = 4;
    sizeOfParts["addressOfEntryPoint"] = 4;
    sizeOfParts["baseOfCode"] = 4;
    sizeOfParts["baseOfData"] = 4;

    // Windows Specific Fields
    sizeOfParts["imageBase"] = 4;
    sizeOfParts["sectionAlignment"] = 4;
    sizeOfParts["fileAlignment"] = 4;
    sizeOfParts["majorOperatingSystemVersion"] = 2;
    sizeOfParts["minorOperatingSystemVersion"] = 2;
    sizeOfParts["majorImageVersion"] = 2;
    sizeOfParts["minorImageVersion"] = 2;
    sizeOfParts["majorSubsystemVersion"] = 2;
    sizeOfParts["minorSubsystemVersion"] = 2;
    sizeOfParts["win32VersionValue"] = 4;
    sizeOfParts["sizeOfImage"] = 4;
    sizeOfParts["sizeOfHeader"] = 4;
    sizeOfParts["checkSum"] = 4;
    sizeOfParts["subSystem"] = 2;
    sizeOfParts["dllChracteristics"] = 2;
    sizeOfParts["sizeOfStackReserve"] = 4;
    sizeOfParts["sizeOfStackCommit"] = 4;
    sizeOfParts["sizeOfHeapReserve"] = 4;
    sizeOfParts["sizeOfHeapCommit"] = 4;
    sizeOfParts["loaderFlags"] = 4;
    sizeOfParts["numberOfRvandSizes"] = 4;

    // Data Directories
    sizeOfParts["DataDirectories"] = 4 * 2 * 16;

    // Section Table
    sizeOfParts["name"] = 8;
    sizeOfParts["virtualSize"] = 4;
    sizeOfParts["virutalAddress"] = 4;
    sizeOfParts["sizeOfRawData"] = 4;
    sizeOfParts["pointerToRawData"] = 4;
    sizeOfParts["pointerToRelocations"] = 4;
    sizeOfParts["pointerToLineNumber"] = 4;
    sizeOfParts["numberOfRelocations"] = 2;
    sizeOfParts["numberOfLineNumbers"] = 2;
    sizeOfParts["characteristicsOfSectionTable"] = 4;
    sizeOfParts["Section"] = 40;
}

void initOffsetOfParts() {
    // COFF Header
    offsetOfParts["signature"] = 0;
    offsetOfParts["machine"] = offsetOfParts["signature"] + sizeOfParts["signature"];
    offsetOfParts["numberOfSections"] = offsetOfParts["machine"] + sizeOfParts["machine"];
    offsetOfParts["timeDateStamp"] = offsetOfParts["numberOfSections"] + sizeOfParts["numberOfSections"];
    offsetOfParts["pointerToSymbolTable"] = offsetOfParts["timeDateStamp"] + sizeOfParts["timeDateStamp"];
    offsetOfParts["numberOfSymbleTable"] = offsetOfParts["pointerToSymbolTable"] + sizeOfParts["pointerToSymbolTable"];
    offsetOfParts["sizeOfOptionalHeader"] = offsetOfParts["numberOfSymbleTable"] + sizeOfParts["numberOfSymbleTable"];
    offsetOfParts["characteristics"] = offsetOfParts["sizeOfOptionalHeader"] + sizeOfParts["sizeOfOptionalHeader"];

    // Standard COFF Fields
    offsetOfParts["magic"] = offsetOfParts["characteristics"] + sizeOfParts["characteristics"];
    offsetOfParts["majorLinkerVersion"] = offsetOfParts["magic"] + sizeOfParts["magic"];
    offsetOfParts["minorLinkerVersion"] = offsetOfParts["majorLinkerVersion"] + sizeOfParts["majorLinkerVersion"];
    offsetOfParts["sizeOfCode"] = offsetOfParts["minorLinkerVersion"] + sizeOfParts["minorLinkerVersion"];
    offsetOfParts["sizeOfInitializedData"] = offsetOfParts["sizeOfCode"] + sizeOfParts["sizeOfCode"];
    offsetOfParts["sizeOfUninitializedData"] = offsetOfParts["sizeOfInitializedData"] + sizeOfParts["sizeOfInitializedData"];
    offsetOfParts["addressOfEntryPoint"] = offsetOfParts["sizeOfUninitializedData"] + sizeOfParts["sizeOfUninitializedData"];
    offsetOfParts["baseOfCode"] = offsetOfParts["addressOfEntryPoint"] + sizeOfParts["addressOfEntryPoint"];
    offsetOfParts["baseOfData"] = offsetOfParts["baseOfCode"] + sizeOfParts["baseOfCode"];

    // Windows Specific Fields
    offsetOfParts["imageBase"] = offsetOfParts["baseOfData"] + sizeOfParts["baseOfData"];
    offsetOfParts["sectionAlignment"] = offsetOfParts["imageBase"] + sizeOfParts["imageBase"];
    offsetOfParts["fileAlignment"] = offsetOfParts["sectionAlignment"] + sizeOfParts["sectionAlignment"];
    offsetOfParts["majorOperatingSystemVersion"] = offsetOfParts["fileAlignment"] + sizeOfParts["fileAlignment"];
    offsetOfParts["minorOperatingSystemVersion"] = offsetOfParts["majorOperatingSystemVersion"] + sizeOfParts["majorOperatingSystemVersion"];
    offsetOfParts["majorImageVersion"] = offsetOfParts["minorOperatingSystemVersion"] + sizeOfParts["minorOperatingSystemVersion"];
    offsetOfParts["minorImageVersion"] = offsetOfParts["majorImageVersion"] + sizeOfParts["majorImageVersion"];
    offsetOfParts["majorSubsystemVersion"] = offsetOfParts["minorImageVersion"] + sizeOfParts["minorImageVersion"];
    offsetOfParts["minorSubsystemVersion"] = offsetOfParts["majorSubsystemVersion"] + sizeOfParts["majorSubsystemVersion"];
    offsetOfParts["win32VersionValue"] = offsetOfParts["minorSubsystemVersion"] + sizeOfParts["minorSubsystemVersion"];
    offsetOfParts["sizeOfImage"] = offsetOfParts["win32VersionValue"] + sizeOfParts["win32VersionValue"];
    offsetOfParts["sizeOfHeader"] = offsetOfParts["sizeOfImage"] + sizeOfParts["sizeOfImage"];
    offsetOfParts["checkSum"] = offsetOfParts["sizeOfHeader"] + sizeOfParts["sizeOfHeader"];
    offsetOfParts["subSystem"] = offsetOfParts["checkSum"] + sizeOfParts["checkSum"];
    offsetOfParts["dllChracteristics"] = offsetOfParts["subSystem"] + sizeOfParts["subSystem"];
    offsetOfParts["sizeOfStackReserve"] = offsetOfParts["dllChracteristics"] + sizeOfParts["dllChracteristics"];
    offsetOfParts["sizeOfStackCommit"] = offsetOfParts["sizeOfStackReserve"] + sizeOfParts["sizeOfStackReserve"];
    offsetOfParts["sizeOfHeapReserve"] = offsetOfParts["sizeOfStackCommit"] + sizeOfParts["sizeOfStackCommit"];
    offsetOfParts["sizeOfHeapCommit"] = offsetOfParts["sizeOfHeapReserve"] + sizeOfParts["sizeOfHeapReserve"];
    offsetOfParts["loaderFlags"] = offsetOfParts["sizeOfHeapCommit"] + sizeOfParts["sizeOfHeapCommit"];
    offsetOfParts["numberOfRvandSizes"] = offsetOfParts["loaderFlags"] + sizeOfParts["loaderFlags"];

    // Data Directories
    offsetOfParts["DataDirectories"] = offsetOfParts["numberOfRvandSizes"] + sizeOfParts["numberOfRvandSizes"];

    // Section Table
    offsetOfParts["name"] = 0;
    offsetOfParts["virtualSize"] = offsetOfParts["name"] + sizeOfParts["name"];
    offsetOfParts["virutalAddress"] = offsetOfParts["virtualSize"] + sizeOfParts["virtualSize"];
    offsetOfParts["sizeOfRawData"] = offsetOfParts["virutalAddress"] + sizeOfParts["virutalAddress"];
    offsetOfParts["pointerToRawData"] = offsetOfParts["sizeOfRawData"] + sizeOfParts["sizeOfRawData"];
    offsetOfParts["pointerToRelocations"] = offsetOfParts["pointerToRawData"] + sizeOfParts["pointerToRawData"];
    offsetOfParts["pointerToLineNumber"] = offsetOfParts["pointerToRelocations"] + sizeOfParts["pointerToRelocations"];
    offsetOfParts["numberOfRelocations"] = offsetOfParts["pointerToLineNumber"] + sizeOfParts["pointerToLineNumber"];
    offsetOfParts["numberOfLineNumbers"] = offsetOfParts["numberOfRelocations"] + sizeOfParts["numberOfRelocations"];
    offsetOfParts["characteristicsOfSectionTable"] = offsetOfParts["numberOfLineNumbers"] + sizeOfParts["numberOfLineNumbers"];
}

void init(){
    initSizeOfParts();
    initOffsetOfParts();
}

void constructIntDataFromRawData() {
    for(int i = 0; i < size; i++) {
        int hex = rawData[i];
        if (hex < 0) {
            hex += 256;
        }
        fileData[i] = hex;
    }
}

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

void showHexOfFile() {
    cout << "Hex dump :" << endl;
    printf("00000000\t\t");
    for(int i = 0; i < size; i++) {
        printf("%02X ", fileData[i]);
        if ((i + 1) % 16 == 0)
            printf("\n%.8X\t\t", i+1);
    }
}

void showInfoOfPEHeader() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    printf("Offset of PE: %.8X \n\n", offsetOfPE);

    string valueToGet;

    valueToGet = "addressOfEntryPoint";
    printf("PointerToEntryPoint: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]));

    valueToGet = "characteristicsOfSectionTable";
    printf("Characteristics: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]));

    valueToGet = "imageBase";
    printf("ImageBase: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]));

    valueToGet = "fileAlignment";
    printf("FileAlignment: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]));

    valueToGet = "sizeOfImage";
    printf("SizeOfImage: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]));

    printf("\n\n");
}

void showInfoOfSection() {
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F

    string valueToGet;

    valueToGet = "numberOfSections";
    int numberOfSection =  getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfPE, sizeOfParts[valueToGet]);
    printf("There are %d sections..\n\n", numberOfSection);
    int sizeOfOneSection = sizeOfParts["Section"];

    valueToGet = "DataDirectories";
    int offsetOfSection = offsetOfParts[valueToGet] + offsetOfPE + sizeOfParts[valueToGet];

    for (int section = 0; section < numberOfSection; section++) {
        printf("Data of section %d: \n", section + 1);
        printf("Offset of section: %.8X\n", offsetOfSection);

        valueToGet = "name";
        string nameOfSection = getRawValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]);
        printf("Name: ");
        for (int i = 0; i < nameOfSection.size(); i++) {
            printf("%c", nameOfSection[i]);
        }
        printf("\n");

        valueToGet = "characteristicsOfSectionTable";
        printf("Characteristics: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]));

        valueToGet = "pointerToRawData";
        printf("RawAddress: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]));

        valueToGet = "sizeOfRawData";
        printf("RawSize: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]));

        valueToGet = "virutalAddress";
        printf("VirtualAddress: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]));

        valueToGet = "virtualSize";
        printf("VirtualSize: %.8X\n", getIntValueFromFileData(offsetOfParts[valueToGet] + offsetOfSection, sizeOfParts[valueToGet]));

        printf("\n\n");

        offsetOfSection += sizeOfOneSection;
    }
}

int main(int argc,  char** argv) {
    filePath = argv[1];
    init();
    ifstream file (filePath.c_str(), ios::in|ios::binary|ios::ate);
    cout << filePath.c_str() << endl;
    if (file.is_open())
    {
        size = file.tellg();
        fileData = vector<int>(size);
        rawData = new char [size];
        file.seekg (0, ios::beg);
        file.read (rawData, size);
        file.close();
        constructIntDataFromRawData();
        if (isPEFile()) {
            showInfoOfPEHeader();
            showInfoOfSection();
            //showHexOfFile();
        } else {
            printf("This a not a PE file\n");
        }
        delete[] rawData;
    }
    else cout << "Unable to open file\n";
    return 0;
}
