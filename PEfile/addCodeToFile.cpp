#include <map>
#include <vector>
#include <string>

#include "initializeValue.h"
#include "getInfoOfFile.h"
#include "addCodeToFile.h"

using namespace std;

void addCodeToFile() {
    outputData = fileData;
    int offsetOfPE = getIntValueFromFileData(60, 4); // 0x3C = 60.... 0X3C -> 0X3F
    int addressOfEntryPoint = getValueOfField("addressOfEntryPoint", offsetOfPE);
    int imageBase = getValueOfField("imageBase", offsetOfPE);
    int fileAlignment = getValueOfField("fileAlignment", offsetOfPE);
    printf("PointerToEntryPoint: %.8X\n", addressOfEntryPoint);
}