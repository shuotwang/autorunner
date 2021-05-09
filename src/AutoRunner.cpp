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
    }else if (Type == "DirectionUp"){
        // upBtn();
    }else if (Type == "DirectionDown") {
        // downBtn();
    }else if (Type == "DirectionLeft") {
        // leftBtn();
    }else if (Type == "DirectionRight") {
        // rightBtn();
    }else if (Type == "uBtn") {
        // uBtn();
    }else if (Type == "iBtn") {
        // iBtn();
    }else if (Type == "jBtn") {
        // jBtn();
    }else if (Type == "kbtn") {
        // kBtn();
    }else if (Type == "cmdBtn") {
        // cmdBtn();
    }else if (Type == "OutputRegs") {
        // outputRegs();
    }else if (Type == "OutputCSRs") {
        // outputCSRs();
    }else if (Type == "OutputMem") {
        OutputMem(MainApp, Data);
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

template <typename T> 
bool OutputMem(T MainApp, std::string &Data) {
    std::cout << "OutputMem" << std::endl;
    return true;
}
