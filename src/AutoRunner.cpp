#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "RISCVConsoleApplication.h"
#include "AutoRunner.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {

    auto MainApp = CRISCVConsoleApplication::Instance("edu.ucdavis.cs.ecs251.riscv-console");
    MainApp->Run(argc, argv);

    FILE* fp = fopen("/code/autograder/src/input.json", "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    // Init
    if (d.HasMember("Init")) {
        int TimerUS = 0, VideoMS = 0, CPUFreq = 0;

        const rapidjson::Value& Init = d["Init"];
        if (Init["TimerUS"].IsInt()) {
            TimerUS = Init["TimerUS"].GetInt(); 
        }

        if (Init["VideoMS"].IsInt()) {
            VideoMS = Init["VideoMS"].GetInt();
        }

        if (Init["CPUFreq"].IsInt()) {
            CPUFreq = Init["CPUFreq"].GetInt();
        }

        if (TimerUS != 0 && VideoMS != 0 && CPUFreq != 0) {
            MainApp->LoadInit("TimerUS", std::to_string(TimerUS));
            MainApp->LoadInit("VideoMS", std::to_string(VideoMS));
            MainApp->LoadInit("CPUFreq", std::to_string(CPUFreq));
        }

    }
    
    // Commands
    if (d.HasMember("Commands")) {
        const rapidjson::Value& Commands = d["Commands"];
        if (Commands.IsArray()) {
            size_t len = Commands.Size();
            for (size_t i = 0; i < len; i++) {
                    
                int Cycle = 0;
                std::string Type = "";
                std::string Data = "";

                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember("Cycle") && CMD["Cycle"].IsInt()) {
                    Cycle = CMD["Cycle"].GetInt();
                }

                if (CMD.HasMember("Type") && CMD["Type"].IsString()) {
                    Type = CMD["Type"].GetString();
                }

                if (CMD.HasMember("Data") && CMD["Data"].IsString()) {
                    Data = CMD["Data"].GetString();
                    Data = Data == "" ? "." : Data;
                    // TODO: get detailed data for OutputMem
                }

                SendCommand(Cycle, Type, Data, MainApp);                    
            }
        }
    }

    
    
}

template <typename T> 
bool SendCommand(int Cycle, std::string &Type, std::string &Data, T MainApp) {
    if (Type == "InsertFW") {
        InsertFW(MainApp, Data);
    }else if (Type == "InsertCart") {
        InsertCR(MainApp, Data);
        DoRun(MainApp);
    }else if (Type == "DirectionUp" || Type == "DirectionDown" || \
                Type == "DirectionLeft" || Type == "DirectionRight"){
        PressDirection(MainApp, Cycle, Type);
    }else if (Type == "UBtn" || Type == "IBtn" || Type == "JBtn" || Type == "KBtn") {
        PressDirection(MainApp, Cycle, Type);
    }else if (Type == "CMDBtn") {
        PressCommand(MainApp);
    }else if (Type == "OutputRegs") {
        OutputRegs(MainApp);
    }else if (Type == "OutputCSRs") {
        OutputCSRs(MainApp);
    }else if (Type == "OutputMem") {
        OutputMem(MainApp);
    }
    return true;
}


template <typename T> 
void InsertFW(T MainApp, std::string &Data){
    if (!MainApp->LoadFW(Data)) {
        std::cout << "Invalid FW Path" << std::endl;
    }
}

template <typename T>
void InsertCR(T MainApp, std::string &Data){
    if (!MainApp->LoadCR(Data)) {
        std::cout << "Invalid CR Path" << std::endl;
    }
}

template <typename T> void RemoveCR(T MainApp) {
    MainApp->RemoveCR();
}

template <typename T> 
void PressDirection(T MainApp, int Cycle, std::string &Data){
    MainApp->PressDirection(Data);
}

template <typename T> 
void ReleaseDirection(T MainApp, std::string &Data) {
    MainApp->ReleaseDirection(Data);
}

template <typename T> 
void PressButton(T MainApp, int Cycle, std::string &Type) {
    MainApp->PressButton(Cycle, Type);
}

template <typename T>
void ReleaseButton(T MainApp, std::string &Data) {
    MainApp->ReleaseNumber(Data);
}

template <typename T> 
void PressCommand(T MainApp) {
    MainApp->PressCommand();
}

template <typename T> void OutputRegs(T MainApp){
    MainApp->OutputRegs();
}

template <typename T> void OutputCSRs(T MainApp){
    MainApp->OutputCSRs();
}

template <typename T> void OutputMem(T MainApp){
    uint32_t addr = 0;
    uint32_t bytes = 16;
    MainApp->OutputMem(addr, bytes);
}

template <typename T> void DoStep(T MainApp) {
    MainApp->DoStep();
}

template <typename T> void DoRun(T MainApp) {
    if(!MainApp->DoRun()) {
        std::cout << "Run Not Working" << std::endl;
    }
}

template <typename T> void DoRunToggle(T MainApp) {
    MainApp->DoRunToggle();
}

template <typename T> void DoPower(T MainApp) {
    MainApp->DoPower();
}

template <typename T> void DoPowerToggle(T MainApp) {
    MainApp->DoPowerToggle();
}