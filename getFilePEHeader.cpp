#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

string filePath = "";
streampos sizeOfFile;
char * rawData;
map<string, int> sizeOfParts;
map<string, int> offsetOfParts;
map<string, int> offsetInDataDicrectory;
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
    sizeOfParts["exportTableRVA"] = 4;
    sizeOfParts["sizeOfExportTable"] = 4;
    sizeOfParts["importTableRVA"] = 4;
    sizeOfParts["sizeOfimportTable"] = 4;

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

    // Import Address Table
    sizeOfParts["IAT"] = 4 * 5;
    sizeOfParts["originalFirstThunk"] = 4;
    sizeOfParts["timeDateStampImport"] = 4;
    sizeOfParts["forwarderChain"] = 4;
    sizeOfParts["name1"] = 4;
    sizeOfParts["firstThunk"] = 4;

    // Thunk Data
    sizeOfParts["ThunkData"] = 4;

    // Import By Name
    sizeOfParts["hint"] = 2;
    sizeOfParts["name1Thunk"] = 8;

    // Export Dicrectory
    sizeOfParts["characteristExport"] = 4;
    sizeOfParts["timeDateStampExport"] = 4;
    sizeOfParts["majorVersion"] = 2;
    sizeOfParts["minorVersion"] = 2;
    sizeOfParts["nName"] = 4;
    sizeOfParts["nBase"] = 4;
    sizeOfParts["numberOfFuntions"] = 4;
    sizeOfParts["numberOfNames"] = 4;
    sizeOfParts["addressOfFuntions"] = 4;
    sizeOfParts["addressOfNames"] = 4;
    sizeOfParts["addressOfNamesOrdinals"] = 4;

    // Name of function Export
    sizeOfParts["nameFunction"] = 4;
    sizeOfParts["ordinal"] = 2;
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
    offsetOfParts["exportTableRVA"] = offsetOfParts["DataDirectories"];
    offsetOfParts["sizeOfExportTable"] = offsetOfParts["exportTableRVA"] + sizeOfParts["exportTableRVA"];
    offsetOfParts["importTableRVA"] = offsetOfParts["sizeOfExportTable"] + sizeOfParts["sizeOfExportTable"];
    offsetOfParts["sizeOfimportTable"] = offsetOfParts["importTableRVA"] + sizeOfParts["importTableRVA"];

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

    // Import Address Table
    offsetOfParts["originalFirstThunk"] = 0;
    offsetOfParts["timeDateStampImport"] = offsetOfParts["originalFirstThunk"] + sizeOfParts["originalFirstThunk"];
    offsetOfParts["forwarderChain"] = offsetOfParts["timeDateStampImport"] + sizeOfParts["timeDateStampImport"];
    offsetOfParts["name1"] = offsetOfParts["forwarderChain"] + sizeOfParts["forwarderChain"];
    offsetOfParts["firstThunk"] = offsetOfParts["name1"] + sizeOfParts["name1"];

    // Thunk Data
    offsetOfParts["ThunkData"] = 0;

    // Import By Name
    offsetOfParts["hint"] = 0;
    offsetOfParts["name1Thunk"] = 2;

    // Export Dicrectory
    offsetOfParts["characteristExport"] = 0;
    offsetOfParts["timeDateStampExport"] = offsetOfParts["characteristExport"] + sizeOfParts["characteristExport"];
    offsetOfParts["majorVersion"] = offsetOfParts["timeDateStampExport"] + sizeOfParts["timeDateStampExport"];
    offsetOfParts["minorVersion"] = offsetOfParts["majorVersion"] + sizeOfParts["majorVersion"];
    offsetOfParts["nName"] = offsetOfParts["minorVersion"] + sizeOfParts["minorVersion"];
    offsetOfParts["nBase"] = offsetOfParts["nName"] + sizeOfParts["nName"];
    offsetOfParts["numberOfFuntions"] = offsetOfParts["nBase"] + sizeOfParts["nBase"];
    offsetOfParts["numberOfNames"] = offsetOfParts["numberOfFuntions"] + sizeOfParts["numberOfFuntions"];
    offsetOfParts["addressOfFuntions"] = offsetOfParts["numberOfNames"] + sizeOfParts["numberOfNames"];
    offsetOfParts["addressOfNames"] = offsetOfParts["addressOfFuntions"] + sizeOfParts["addressOfFuntions"];
    offsetOfParts["addressOfNamesOrdinals"] = offsetOfParts["addressOfNames"] + sizeOfParts["addressOfNames"];

    // Name of function Export
    offsetOfParts["nameFunction"] = 0;
    offsetOfParts["ordinal"] = 0;
}

void init(){
    initSizeOfParts();
    initOffsetOfParts();
}

void constructIntDataFromRawData() {
    for(int i = 0; i < sizeOfFile; i++) {
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

string getStringFromFileData(int offset) {
    string res = "";
    while (fileData[offset] != 0) {
        res += fileData[offset];
        offset++;
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

    string valueToGet;
    int numberOfSection = getValueOfField("numberOfSections", offsetOfPE);
    int sizeOfOneSection = sizeOfParts["Section"];
    valueToGet = "DataDirectories";
    int offsetOfSection = offsetOfParts[valueToGet] + offsetOfPE + sizeOfParts[valueToGet];
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

    int diffRVAandOffset = offsetImportTable - RVAImportTable;

    while (true) {
        int RVAName1 = getValueOfField("name1", offsetImportTable);
        int RVAFirstThunk = getValueOfField("firstThunk", offsetImportTable);   
        if (RVAName1 == 0 && RVAFirstThunk == 0) {
            return;
        }
        int offsetName1 = RVAName1 + diffRVAandOffset;
        int offsetFirstThunk = RVAFirstThunk + diffRVAandOffset;
        cout << getStringFromFileData(offsetName1) << endl;

        int RVAOriginalFirstThunk = getValueOfField("originalFirstThunk", offsetImportTable); 
        // printf("RVAOriginalFirstThunk: %.8X\n", RVAOriginalFirstThunk);
        // printf("RVAFirstThunk: %.8X\n", RVAFirstThunk);
        // printf("offsetFirstThunk: %.8X\n", offsetFirstThunk);
        // Thunk data
        while (true) {
            int RVAImportByName = getValueOfField("ThunkData", offsetFirstThunk);
            int offsetImportByName = RVAImportByName + diffRVAandOffset;
            // printf("offsetImportByName: %.8X\n", offsetImportByName);
            if (offsetImportByName > fileData.size()) {
                // bug when test with dll file.....
                // @TODO
                offsetFirstThunk += sizeOfParts["ThunkData"];
                break;
            }
            if (RVAImportByName == 0) {
                break;
            }
            offsetImportByName += offsetOfParts["name1Thunk"];
            cout << "\t" << getStringFromFileData(offsetImportByName) << endl;

            offsetFirstThunk += sizeOfParts["ThunkData"];
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
        printf("\t\tOrdinal: %.4X\tName: ", ordinal); cout << getStringFromFileData(offsetOfName) << endl;
    }

    cout << "\tThere are " << numberOfFunctions - numberOfNames << " named functions: " << endl;
    for (int indexOfOrdinalFunction = numberOfNames; indexOfOrdinalFunction < numberOfFunctions; indexOfOrdinalFunction++) {
        int ordinal = getValueOfField("ordinal", offsetOfNameOrdinals + indexOfOrdinalFunction * sizeOfParts["ordinal"]);
        printf("\t\tOrdinal: %.4X\n", ordinal);
    }
}

int main(int argc,  char** argv) {
    filePath = argv[1];
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/BASECALC/BASECALC.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/FAKE.EXE";
    // filePath = "/Users/macbook/Desktop/SharedWithWindows/twain_32.dll";
    
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
