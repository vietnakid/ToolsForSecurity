#include <iostream>
#include <vector>

#include "getValueFromFile.h"

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

vector<int> getRawValueFromFileData(int offset, int size) {
    vector<int> res;
    for(int i = 0; i < size; i++) {
        res.push_back(fileData[i + offset]);
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