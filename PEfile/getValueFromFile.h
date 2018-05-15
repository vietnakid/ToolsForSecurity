#ifndef GET_VALUE_FROM_FILE
#define GET_VALUE_FROM_FILE

#include <map>
#include "initializeValue.h"

using namespace std;

extern vector<int> fileData;
extern streampos sizeOfFile;

int getIntValueFromFileData(int offset, int size);
vector<int> getRawValueFromFileData(int offset, int size);
string getStringFromFileData(int offset);

#endif