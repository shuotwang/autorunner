#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "RISCVConsoleApplication.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "CommandsResponder.h"


int main(int argc, char *argv[]) {

    auto MainApp = CRISCVConsoleApplication::Instance("edu.ucdavis.cs.ecs251.riscv-console");
    MainApp->Run(argc, argv);

    int TimerUS = 0, VideoMS = 0, CPUFreq = 0;
    int Cycle = 0;
    std::string Type = "";
    std::string Data = "";

    FILE* fp = fopen("/code/autograder/src/input.json", "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    // Init
    if (d.HasMember("Init")) {

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
                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember("Cycle") && CMD["Cycle"].IsInt()) {
                    Cycle = CMD["Cycle"].GetInt();
                }

                if (CMD.HasMember("Type") && CMD["Type"].IsString()) {
                    Type = CMD["Type"].GetString();
                }

                if (CMD.HasMember("Data") && CMD["Data"].IsString()) {
                    Data = CMD["Data"].GetString();
                    // TODO: get detailed data for OutputMem
                }
                // TODO: Respond to Commands
            }
        }
    }

    // pass the params into Commands Responder
    if (Cycle && !Type.empty() && !Data.empty()) {
        sendCommand(Cycle, Type, Data);
    }

    
    // Reading finishes
    
}