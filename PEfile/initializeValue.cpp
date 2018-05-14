#include <map>
#include <string>

#include "initializeValue.h"

using namespace std;

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
    sizeOfParts["addressOfFunction"] = 4;
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
    offsetOfParts["addressOfFunction"] = 0;
}

void init(){
    initSizeOfParts();
    initOffsetOfParts();
}