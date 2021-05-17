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
#include <map>


template <typename T> 
bool SendCommand(int Cycle, std::string &Type, std::string &Data, T MainApp, rapidjson::Document::AllocatorType &Allocator, rapidjson::Value &valueObjectArray);

template <typename T> void InsertFW(T MainApp, std::string &Data);
template <typename T> void InsertCR(T MainApp, std::string &Data);
template <typename T> void RemoveCR(T MainApp);

template <typename T> void PressDirection(T MainApp, int Cycle, std::string &Data);
template <typename T> void ReleaseDirection(T MainApp, std::string &Data);
template <typename T> void PressButton(T MainApp, int Cycle, std::string &Type);
template <typename T> void ReleaseButton(T MainApp, std::string &Data);
template <typename T> void CMDBtn(T MainApp, int Cycle, std::string &Data);

template <typename T> std::map<std::string, std::string> OutputRegs(T MainApp, int Cycle);
template <typename T> std::map<std::string, std::string> OutputCSRs(T MainApp, int Cycle);
template <typename T> std::map<std::string, std::string> OutputMem(T MainApp, std::string &Data, int Cycle);

template <typename T> void DoStep(T MainApp);
template <typename T> void DoRun(T MainApp);
template <typename T> void DoRunToggle(T MainApp);
template <typename T> void DoPower(T MainApp);
template <typename T> void DoPowerToggle(T MainApp);

rapidjson::Value FormatOutputMap(std::map<std::string, std::string> Map, rapidjson::Document::AllocatorType &Allocator);

uint32_t GetAddHex(std::string strAdd);


#endif