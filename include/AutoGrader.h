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

// class AutoGrader {
//     protected:
//         std::string InputJsonPath;

// };



template <typename T> 
bool SendCommand(int Cycle, std::string &Type, std::string &Data, T MainApp);

template <typename T> void InsertFW(T MainApp, std::string &Data);
template <typename T> void InsertCR(T MainApp, std::string &Data);
template <typename T> void RemoveCR(T MainApp);

template <typename T> void PressDirection(T MainApp, int Cycle, std::string &Data);
template <typename T> void ReleaseDirection(T MainApp, std::string &Data);
template <typename T> void PressButton(T MainApp, int Cycle, std::string &Type);
template <typename T> void ReleaseButton(T MainApp, std::string &Data);
template <typename T> void CMDBtn(T MainApp, int Cycle, std::string &Data);

template <typename T> void OutputRegs(T MainApp);
template <typename T> void OutputCSRs(T MainApp);
template <typename T> void OutputMem(T MainApp);

template <typename T> void DoStep(T MainApp);
template <typename T> void DoRun(T MainApp);
template <typename T> void DoRunToggle(T MainApp);
template <typename T> void DoPower(T MainApp);
template <typename T> void DoPowerToggle(T MainApp);


#endif