#ifndef GET_INFO_OF_FILE_H
#define GET_INFO_OF_FILE_H

#include<map>
#include "initializeValue.h"

using namespace std;

extern vector<int> fileData;
extern streampos sizeOfFile;

int getIntValueFromFileData(int offset, int size);
string getRawValueFromFileData(int offset, int size);
string getStringFromFileData(int offset);
void showHexOfFile();
int getValueOfField(string field, int baseOffset);
void showInfoOfPEHeader();
void showInfoOfSection();
void showImports();
void showExports();
void showInfoOfFile();

#endif