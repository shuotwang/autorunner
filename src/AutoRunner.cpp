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

    int timerUS = 0, videoMS = 0, CPUFreq = 0;
    int cycle = 0;
    std::string type = "";
    std::string data = "";

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
            timerUS = Init["TimerUS"].GetInt(); 
        }

        if (Init["VideoMS"].IsInt()) {
            videoMS = Init["VideoMS"].GetInt();
        }

        if (Init["CPUFreq"].IsInt()) {
            CPUFreq = Init["CPUFreq"].GetInt();
        }

        // TODO: do init operations
        printf("[Init]:\ntimerUS: %d, videoMS: %d, CPUFreq: %d\n", timerUS, videoMS, CPUFreq);
    }
    
    // Commands
    if (d.HasMember("Commands")) {
        const rapidjson::Value& Commands = d["Commands"];
        if (Commands.IsArray()) {
            size_t len = Commands.Size();
            for (size_t i = 0; i < len; i++) {
                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember("Cycle") && CMD["Cycle"].IsInt()) {
                    cycle = CMD["Cycle"].GetInt();
                }

                if (CMD.HasMember("Type") && CMD["Type"].IsString()) {
                    type = CMD["Type"].GetString();
                }

                if (CMD.HasMember("Data") && CMD["Data"].IsString()) {
                    data = CMD["Data"].GetString();
                    // TODO: get detailed data for OutputMem
                }

                printf("[Commands]:\ncycle: %d, type: %s, data: %s\n", cycle, type.c_str(), data.c_str());

                // TODO: Respond to Commands
            }
        }
    }

    // pass the params into Commands Responder
    if (cycle && !type.empty() && !data.empty()) {
        sendCommand(cycle, type, data);
    }

    
    // Reading finishes
    
}