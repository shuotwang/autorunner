#ifndef AUTO_RUNNER_H
#define AUTO_RUNNER_H

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "RISCVConsole.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <memory>

class AutoGrader {
    protected:
        char *InputJSONPath;
        char *OutputJSONPath;
        static std::unordered_map<uint32_t, std::string> CSRMap;

        rapidjson::Document InputJSONDocument; 
        rapidjson::Document OutputJSONDocument;
        rapidjson::Value OutputJSONObjectArray;

        std::shared_ptr<CRISCVConsole> DRISCVConsole;

        rapidjson::Document GetInputJSONDocument(char* InputJSONPath);

        void ParseInitData();
        void ParseCommandData();
        void SendCommand(uint32_t Cycle, std::string &Type, std::string &Data);

        void OutputJSONFile();

        bool InsertFW(std::string &Data);
        bool InsertCR(std::string &Data);
        bool RemoveCR();

        bool PressDirection(std::string &Data);
        bool ReleaseDirection(std::string &Data);
        bool PressButton(std::string &Type);
        bool ReleaseButton(std::string &Data);
        bool PressCommand();

        std::map<std::string, std::string> OutputRegs();
        std::map<std::string, std::string> OutputCSRs();
        std::map<std::string, std::string> OutputMem(std::string &Data);

        bool DoStep();
        bool DoRun();
        bool DoStop();
        bool DoPowerOn();
        bool DoPowerOff();

        uint32_t GetAddHex(std::string strAdd);
        rapidjson::Value FormatOutputMap(std::map<std::string, std::string> Map, rapidjson::Document::AllocatorType &Allocator);
        std::string FormatHex32Bit(uint32_t val);
        
    public:
        explicit AutoGrader(char *InputJSONPath, char *OutputJSONPath);

};


#endif