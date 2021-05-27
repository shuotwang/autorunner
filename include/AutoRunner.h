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

class CAutoRunner {
    protected:
        static const std::string INIT_STRING;
        static const std::string TIMER_US_STRING;
        static const std::string VIDEO_MS_STRING;
        static const std::string CPU_FREQ_STRING;

        static const std::string COMMANDS_STRING;
        static const std::string CYCLE_STRING;
        static const std::string TYPE_STRING;
        static const std::string DATA_STRING;

        static const std::string DIRECTION_UP_STRING;
        static const std::string DIRECTION_DOWN_STRING;
        static const std::string DIRECTION_LEFT_STRING;
        static const std::string DIRECTION_RIGHT_STRING;

        static const std::string U_BUTTON_STRING;
        static const std::string I_BUTTON_STRING;
        static const std::string J_BUTTON_STRING;
        static const std::string K_BUTTON_STRING;

        static const std::string INSERT_FW_STRING;
        static const std::string INSERT_CR_STRING;
        static const std::string REMOVE_CR_STRING;

        static const std::string CMD_BUTTON_STRING;
        
        static const std::string OUTPUT_REG_STRING;
        static const std::string OUTPUT_CSR_STRING;
        static const std::string OUTPUT_MEM_STRING;
        

        std::string DJSONFilePath = "/code/autorunner/files/";
        std::string DInputJSONPath;
        std::string DOutputJSONPath;

        rapidjson::Document DInputJSONDocument; 
        rapidjson::Document DOutputJSONDocument;
        rapidjson::Value DOutputJSONObjectArray;

        std::shared_ptr<CRISCVConsole> DRISCVConsole;

        rapidjson::Document GetInputJSONDocument();

        void ParseArguments(int &argc, char *argv[]);
        void ParseInitData();
        void ParseCommandData();
        void SendCommand(uint32_t cycle, uint32_t nextCycle, std::string &type, std::string &data);

        void OutputJSONFile();

        bool InsertFW(std::string &data);
        bool InsertCR(std::string &data);
        bool RemoveCR();

        bool PressDirection(std::string &type);
        bool ReleaseDirection(std::string &type);
        bool PressButton(std::string &type);
        bool ReleaseButton(std::string &type);
        bool PressCommand();
        
        bool IsDirectionButton(std::string &type);
        bool IsNumberButton(std::string &type);

        std::map<std::string, std::string> OutputRegs();
        std::map<std::string, std::string> OutputCSRs();
        std::map<std::string, std::string> OutputMem(std::string &data);

        bool DoStep();
        bool DoRun();
        bool DoStop();
        bool DoPowerOn();
        bool DoPowerOff();
        bool DoCycleSteps(uint32_t cycle, uint32_t nextCycle);

        uint32_t GetAddHex(std::string strAdd);
        rapidjson::Value FormatOutputMap(std::map<std::string, std::string> map, rapidjson::Document::AllocatorType &allocator);
        std::string FormatHex32Bit(uint32_t val);
        
    public:
        explicit CAutoRunner(int argc, char *argv[]);

};


#endif