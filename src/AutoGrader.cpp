#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/filewritestream.h"
#include "RISCVConsoleApplication.h"
#include "AutoGrader.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <map>

AutoGrader::AutoGrader(std::string &InputJSONPath, std::string &OutputJSONPath) {
    AutoGrader::InputJSONPath = InputJSONPath;
    AutoGrader::OutputJSONPath = OutputJSONPath;

    InputJSONDocument = GetInputJSONDocument(InputJSONPath);
    
    OutputJSONDocument.SetObject();
    OutputAllocator = OutputJSONDocument.GetAllocator();
    OutputJSONObjectArray(rapidjson::kArrayType);
}

rapidjson::Document AutoGrader::GetInputJSONDocument(std::string &InputJSONPath) {
    FILE* fp = fopen(InputJSONPath, "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    return d;
}

void AutoGrader::ParseInitData() {
    if (InputJSONDocument.HasMember("Init")) {
        uint32_t TimerUS = 0, VideoMS = 0, CPUFreq = 0;

        const rapidjson::Value& Init = InputJSONDocument["Init"];
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
            CRISCVConsole(TimerUS, VideoMS, CPUFreq);
        }
    }
}

void AutoGrader::ParseCommandData() {
    if (InputJSONDocument.HasMember("Commands")) {
        uint32_t Cycle = 0;
        const rapidjson::Value& Commands = InputJSONDocument["Commands"];
        if (Commands.IsArray()) {
            size_t len = Commands.Size();
            for (size_t i = 0; i < len; i++) {

                std::string Type = "";
                std::string Data = "";

                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember("Cycle") && CMD["Cycle"].IsInt()) {
                    int CurrentCycle = CMD["Cycle"].GetInt();
                    while (Cycle < CurrentCycle) {
                        CRISCVConsole->DoStep();
                        Cycle++;
                    }
                }

                if (CMD.HasMember("Type") && CMD["Type"].IsString()) {
                    Type = CMD["Type"].GetString();
                }

                if (CMD.HasMember("Data") && CMD["Data"].IsString()) {
                    Data = CMD["Data"].GetString();
                    Data = Data == "" ? "." : Data;
                }

                SendCommand(Cycle, Type, Data);                    
            }
        }
    }
}


void AutoGrader::Run() {
    
    // AutoGrader::InputJSONPath = "/code/autograder/files/input.json";
    // AutoGrader::OutputJSONPath = "/code/autograder/files/output.json";

    
}

void AutoGrader::OutputJSONFile() {
    output.AddMember("Outputs", OutputJSONObjectArray, OutputAllocator);
    FILE* f = fopen(OutputJSONPath, "w");
	char writeBuffer[65535];
	rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	output.Accept(writer);
	fclose(f);
}


int main(int argc, char *argv[]) {
    



    
}

template <typename T> 
void SendCommand(int Cycle, std::string &Type, std::string &Data, T MainApp, rapidjson::Document::AllocatorType &Allocator, rapidjson::Value &valueObjectArray) {
    if (Type == "InsertFW") {
        InsertFW(MainApp, Data);
        DoPower(MainApp);
    }else if (Type == "InsertCart") {
        InsertCR(MainApp, Data);
    }else if (Type == "RemoveCart") {
        RemoveCR(MainApp);
    }else if (Type == "DirectionUp" || Type == "DirectionDown" || \
                Type == "DirectionLeft" || Type == "DirectionRight"){
        PressDirection(MainApp, Cycle, Type);
    }else if (Type == "UBtn" || Type == "IBtn" || Type == "JBtn" || Type == "KBtn") {
        PressButton(MainApp, Cycle, Type);
    }else if (Type == "CMDBtn") {
        PressCommand(MainApp);
    }else if (Type == "OutputRegs") {
        std::map<std::string, std::string> outRegs = OutputRegs(MainApp, Cycle);
        rapidjson::Value outRegJsonVal = FormatOutputMap(outRegs, Allocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, Allocator);
        temp.AddMember("Regs", outRegJsonVal, Allocator);
        valueObjectArray.PushBack(temp, Allocator);
    }else if (Type == "OutputCSRs") {
        std::map<std::string, std::string> outCSRs = OutputCSRs(MainApp, Cycle);
        rapidjson::Value outCSRJsonVal = FormatOutputMap(outCSRs, Allocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, Allocator);
        temp.AddMember("CSRs", outCSRJsonVal, Allocator);
        valueObjectArray.PushBack(temp, Allocator);
    }else if (Type == "OutputMem") {
        std::map<std::string, std::string> outMem = OutputMem(MainApp, Data, Cycle);
        rapidjson::Value outMemJsonVal = FormatOutputMap(outMem, Allocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, Allocator);
        temp.AddMember("Mem", outMemJsonVal, Allocator);
        valueObjectArray.PushBack(temp, Allocator);
    }
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
void PressButton(T MainApp, int Cycle, std::string &Data) {
    MainApp->PressNumber(Data);
}

template <typename T>
void ReleaseButton(T MainApp, std::string &Data) {
    MainApp->ReleaseNumber(Data);
}

template <typename T> 
void PressCommand(T MainApp) {
    MainApp->PressCommand();
}

template <typename T> std::map<std::string, std::string> OutputRegs(T MainApp, int Cycle){
    std::cout << "AutoGrader Output Regs" << std::endl;
    return MainApp->OutputRegs(Cycle);
}

template <typename T> std::map<std::string, std::string> OutputCSRs(T MainApp, int Cycle){
    std::cout << "AutoGrader Output CSRs" << std::endl;
    return MainApp->OutputCSRs(Cycle);
}

template <typename T> std::map<std::string, std::string> OutputMem(T MainApp, std::string &Data, int Cycle){
    std::cout << "AutoGrader Output Mem" << std::endl;
    // reference from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    std::string delim = "-";

    auto start = 0U;
    auto end = Data.find(delim);
    std::string startAdd;
    std::string endAdd;
    
    while (end != std::string::npos)
    {
        startAdd = Data.substr(start, end - start);
        start = end + delim.length();
        end = Data.find(delim, start);
    }

    endAdd = Data.substr(start, end);

    uint32_t startAddHex = GetAddHex(startAdd);
    uint32_t bytes = GetAddHex(endAdd) - startAddHex + 1;
    return {{startAdd, MainApp->OutputMem(startAddHex, bytes, Cycle)}};

    
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

uint32_t GetAddHex(std::string strAdd){

    // reference from: https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << strAdd;
    ss >> x;
    // output it as a signed type
    uint32_t addr = static_cast<int>(x);
    return addr;
}

rapidjson::Value FormatOutputMap(std::map<std::string, std::string> Map, rapidjson::Document::AllocatorType &Allocator){
    rapidjson::Value root(rapidjson::kObjectType);

    rapidjson::Value key(rapidjson::kStringType);  
    rapidjson::Value value(rapidjson::kStringType); 
 
	for(std::map<std::string, std::string>::const_iterator it = Map.begin(); it != Map.end(); ++it){
		key.SetString(it->first.c_str(), Allocator);  
   		value.SetString(it->second.c_str(), Allocator);  
    	root.AddMember(key, value, Allocator);
	}

    return root;
}