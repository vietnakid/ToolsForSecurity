#ifndef GET_INFO_OF_FILE_H
#define GET_INFO_OF_FILE_H

#include <map>
#include "initializeValue.h"
#include "getValueFromFile.h"

using namespace std;

extern vector<int> fileData;
extern streampos sizeOfFile;

void showHexOfFile();
int getValueOfField(string field, int baseOffset);
void showInfoOfPEHeader();
void showInfoOfSection();
void showImports();
void showExports();
void showInfoOfFile();
void getImportSectionData();

#endif