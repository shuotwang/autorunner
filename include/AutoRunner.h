#ifndef AUTO_RUNNER_H
#define AUTO_RUNNER_H

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

template <typename T> void PressDirection(T MainApp, int Cycle, std::string &Data);
template <typename T> void UBtn(T MainApp, int Cycle, std::string &Data);
template <typename T> void IBtn(T MainApp, int Cycle, std::string &Data);
template <typename T> void JBtn(T MainApp, int Cycle, std::string &Data);
template <typename T> void KBtn(T MainApp, int Cycle, std::string &Data);
template <typename T> void CMDBtn(T MainApp, int Cycle, std::string &Data);

template <typename T> void OutputRegs(T MainApp);
template <typename T> void OutputCSRs(T MainApp);
template <typename T> void OutputMem(T MainApp);


#endif