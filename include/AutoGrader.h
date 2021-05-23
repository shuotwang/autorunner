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

class AutoGrader {
    protected:
        std::string InputJSONPath;
        std::string OutputJSONPath;

        rapidjson::Document InputJSONDocument; 
        rapidjson::Document OutputJSONDocument;
        rapidjson::Value OutputJSONObjectArray;
        rapidjson::Document::AllocatorType &OutputAllocator;

        std::shared_ptr<CRISCVConsole> DRISCVConsole;

        rapidjson::Document GetInputJSONDocument(std::string &InputJSONPath);

        void ParseInitData();
        void ParseCommandData();
        void SendCommand(uint32_t Cycle, std::string &Type, std::string &Data);
        

        void OutputJSONFile();

        
    public:
        explicit AutoGrader(std::string &InputJSONPath, std::string &OutputJSONPath);

        void InsertFW(std::string &Data);
        void InsertCR(std::string &Data);
        void RemoveCR();

        void PressDirection(std::string &Data);
        void ReleaseDirection(std::string &Data);
        void PressButton(std::string &Type);
        void ReleaseButton(std::string &Data);
        void CMDBtn(std::string &Data);

        std::map<std::string, std::string> OutputRegs();
        std::map<std::string, std::string> OutputCSRs();
        std::map<std::string, std::string> OutputMem(&Data);

        void DoStep();
        void DoRun();
        void DoStop();
        void DoPowerUp();
        void DoPowerDown();


        
}


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