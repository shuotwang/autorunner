#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "RISCVConsoleApplication.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>


template <typename T> 
bool SendCommand(int Cycle, std::string &Type, std::string &Data, T MainApp);

template <typename T> void InsertFW(T MainApp, std::string &Data);
template <typename T> void InsertCR(T MainApp, std::string &Data);

template <typename T> bool OutputMem(T MainApp, std::string &Data);