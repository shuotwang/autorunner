#include "AutoGrader.h"
#include <iostream>

int main(int argc, char *argv[]){
    char *InputJSONPath = (char*)"/code/autograder/files/input.json";
    char *OutputJSONPath = (char*)"/code/autograder/files/output.json";
    AutoGrader(InputJSONPath, OutputJSONPath);
    return 0;
}