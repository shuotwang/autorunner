#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/filewritestream.h"
#include "RISCVConsoleApplication.h"
#include "AutoRunner.h"
#include "RISCVConsole.h"
#include "FileDataSink.h"
#include "FileDataSource.h"
#include "Path.h"
#include "RISCVSYSTypeInstruction.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <map>


AutoRunner::AutoRunner(char *InputJSONPath, char *OutputJSONPath){
    AutoRunner::InputJSONPath = InputJSONPath;
    AutoRunner::OutputJSONPath = OutputJSONPath;

    InputJSONDocument = GetInputJSONDocument(InputJSONPath);
    ParseInitData();

    OutputJSONDocument.SetObject();
    rapidjson::Value TempValue(rapidjson::kArrayType);
    OutputJSONObjectArray = TempValue;

    ParseCommandData();
    OutputJSONFile();
}

rapidjson::Document AutoRunner::GetInputJSONDocument(char *InputJSONPath) {
    FILE* fp = fopen(InputJSONPath, "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    return d;
}

void AutoRunner::ParseInitData() {
    uint32_t TimerUS = 0, VideoMS = 0, CPUFreq = 0;

    if (InputJSONDocument.HasMember("Init")) {
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
    }
    DRISCVConsole = std::make_shared<CRISCVConsole>(TimerUS, VideoMS, CPUFreq);
    DRISCVConsole->SetDebugMode(true);
}

void AutoRunner::ParseCommandData() {
    if (InputJSONDocument.HasMember("Commands")) {
        const rapidjson::Value& Commands = InputJSONDocument["Commands"];
        if (Commands.IsArray()) {
            size_t len = Commands.Size();
            for (size_t i = 0; i < len; i++) {
                uint32_t CurrentCycle = 0;
                uint32_t NextCycle = 0;
                std::string Type = "";
                std::string Data = "";

                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember("Cycle") && CMD["Cycle"].IsInt()) {
                    CurrentCycle = CMD["Cycle"].GetInt();
                }

                if (CMD.HasMember("Type") && CMD["Type"].IsString()) {
                    Type = CMD["Type"].GetString();
                }

                if (CMD.HasMember("Data") && CMD["Data"].IsString()) {
                    Data = CMD["Data"].GetString();
                    Data = Data == "" ? "." : Data;
                }
                
                if (i + 1 < len) {
                    const rapidjson::Value &NextCMD = Commands[i+1];
                    if (NextCMD.HasMember("Cycle") && NextCMD["Cycle"].IsInt()) {
                        NextCycle = NextCMD["Cycle"].GetInt();
                    }
                }else {
                    NextCycle = CurrentCycle;
                }
                SendCommand(CurrentCycle, NextCycle, Type, Data);                    
            }
        }
    }
}

bool AutoRunner::IsDirectionButton(std::string &Type) {
    if (Type == "DirectionUp" || Type == "DirectionDown" || \
        Type == "DirectionLeft" || Type == "DirectionRight") {
            return true;
        }
    return false;
}

bool AutoRunner::IsNumberButton(std::string &Type) {
    if (Type == "UBtn" || Type == "IBtn" || Type == "JBtn" || Type == "KBtn"){
        return true;
    }
    return false;
}
    
void AutoRunner::OutputJSONFile() {
    rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
    OutputJSONDocument.AddMember("Outputs", OutputJSONObjectArray, OutputAllocator);
    FILE* f = fopen(OutputJSONPath, "w");
	char writeBuffer[65535];
	rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	OutputJSONDocument.Accept(writer);
	fclose(f);
}

void AutoRunner::SendCommand(uint32_t Cycle, uint32_t NextCycle, std::string &Type, std::string &Data) {
    if (Type == "InsertFW") {
        InsertFW(Data);
        DoPowerOn();
        DoCycleSteps(Cycle, NextCycle);
    }else if (Type == "InsertCart") {
        InsertCR(Data);
        DoCycleSteps(Cycle, NextCycle);
    }else if (Type == "RemoveCart") {
        RemoveCR();
        DoCycleSteps(Cycle, NextCycle);
    }else if (IsDirectionButton(Type) || IsNumberButton(Type)) {
        for (uint32_t i=0; i<(NextCycle - Cycle); i++) {
            if (IsDirectionButton(Type)){
                PressDirection(Type);
            }

            if (IsNumberButton(Type)){
                PressDirection(Type);
            }
            DoStep();
        }
        ReleaseDirection(Type);
        ReleaseButton(Type);
    }else if (Type == "CMDBtn") {
        PressCommand();
        DoCycleSteps(Cycle, NextCycle);
    }else if (Type == "OutputRegs") {
        std::map<std::string, std::string> outRegs = OutputRegs();
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outRegJsonVal = FormatOutputMap(outRegs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("Regs", outRegJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
        DoCycleSteps(Cycle, NextCycle);
    }else if (Type == "OutputCSRs") {
        std::map<std::string, std::string> outCSRs = OutputCSRs();
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outCSRJsonVal = FormatOutputMap(outCSRs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("CSRs", outCSRJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
        DoCycleSteps(Cycle, NextCycle);
    }else if (Type == "OutputMem") {
        std::map<std::string, std::string> outMem = OutputMem(Data);
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outMemJsonVal = FormatOutputMap(outMem, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("Mem", outMemJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
        DoCycleSteps(Cycle, NextCycle);
    }
}


bool AutoRunner::InsertFW(std::string &Data){
    if (!Data.empty()){
        std::string FWFileName = Data;

        auto InFile = std::make_shared<CFileDataSource>(FWFileName);
        if(DRISCVConsole->ProgramFirmware(InFile)){
            return true;
        }
    }
    return false;
}

bool AutoRunner::InsertCR(std::string &Data){
    if (!Data.empty()){
        std::string CRFileName = Data;

        auto InFile = std::make_shared<CFileDataSource>(CRFileName);
        if(DRISCVConsole->InsertCartridge(InFile)){
            return true;
        }
    }
    return false;
}

bool AutoRunner::RemoveCR() {
    DRISCVConsole->RemoveCartridge();
    return true;
}

bool AutoRunner::PressDirection(std::string &Data){
    if (!Data.empty()) {
        if (Data == "DirectionUp") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Up);
            return true;
        } else if (Data == "DirectionDown") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Down);
            return true;
        } else if (Data == "DirectionLeft") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Left);
            return true;
        } else if (Data == "DirectionRight") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Right);
            return true;
        }
    }
    return false;
}

bool AutoRunner::ReleaseDirection(std::string &Data) {
    if (!Data.empty()) {
        if (Data == "DirectionUp") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Up);
            return true;
        } else if (Data == "DirectionDown") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Down);
            return true;
        } else if (Data == "DirectionLeft") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Left);
            return true;
        } else if (Data == "DirectionRight") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Right);
            return true;
        }
    }
    return false;
}

bool AutoRunner::PressButton(std::string &Data) {
    if (!Data.empty()) {
        if (Data == "UBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button1);
            return true;
        }else if (Data == "IBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button2);
            return true;
        }else if (Data == "JBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button3);
            return true;
        }else if (Data == "KBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button4);
            return true;
        }
    }
    return false;
}

bool AutoRunner::ReleaseButton(std::string &Data) {
    if (!Data.empty()) {
        if (Data == "UBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button1);
            return true;
        }else if (Data == "IBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button2);
            return true;
        }else if (Data == "JBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button3);
            return true;
        }else if (Data == "KBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button4);
            return true;
        }
    }
    return false;
}

bool AutoRunner::PressCommand() {
    DRISCVConsole->PressCommand();
    return true;
}

std::map<std::string, std::string> AutoRunner::OutputRegs(){
    std::map<std::string, std::string> RegOutputMap;

    for(size_t Index = 0; Index < CRISCVCPU::RegisterCount(); Index++){
        auto RegisterName = CRISCVCPU::CInstruction::RegisterName(Index);
        RegOutputMap[RegisterName] = FormatHex32Bit(DRISCVConsole->CPU()->Register(Index));
    }

    return RegOutputMap;
}

std::map<std::string, std::string> AutoRunner::OutputCSRs(){
    std::map<std::string, std::string> CSROutputMap;
    size_t LineIndex = 0;
    for(uint32_t CSRAddr : DRISCVConsole->CPU()->ControlStatusRegisterKeys()){
        std::string CSRName = CRISCVSYSTypeInstruction::ControlStatusRegisterName(CSRAddr);
        CSROutputMap[CSRName] = FormatHex32Bit(DRISCVConsole->CPU()->ControlStatusRegister(CSRAddr));
        LineIndex++;
    }
    return CSROutputMap;
}

std::map<std::string, std::string> AutoRunner::OutputMem(std::string &Data){
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

    std::stringstream OutputStream;
    const uint8_t *buffer = DRISCVConsole->Memory()->LoadData(startAddHex, bytes);

    for(uint32_t Index = 0; Index < bytes; Index++){
        OutputStream<<std::setfill('0') << std::setw(2) << std::hex << uint32_t(buffer[Index]);
    }
    return {{startAdd, OutputStream.str()}};
}

bool AutoRunner::DoStep() {
    DRISCVConsole->Step();
    return true;
}

bool AutoRunner::DoRun() {
    DRISCVConsole->Run();
    return true;
}

bool AutoRunner::DoStop() {
    DRISCVConsole->Stop();
    return true;
}

bool AutoRunner::DoPowerOn() {
    DRISCVConsole->PowerOn();
    return true;
}

bool AutoRunner::DoPowerOff() {
    DRISCVConsole->PowerOff();
    return true;
}

bool AutoRunner::DoCycleSteps(uint32_t Cycle, uint32_t NextCycle){
    for (uint32_t i=0; i < (NextCycle - Cycle); i++) {
        DoStep();
    }
    return true;
}   

uint32_t AutoRunner::GetAddHex(std::string strAdd){
    // reference from: https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << strAdd;
    ss >> x;
    // output it as a signed type
    uint32_t addr = static_cast<int>(x);
    return addr;
}

rapidjson::Value AutoRunner::FormatOutputMap(std::map<std::string, std::string> Map, rapidjson::Document::AllocatorType &Allocator){
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

std::string AutoRunner::FormatHex32Bit(uint32_t val){
    std::stringstream Stream;
    Stream<<std::setfill('0') << std::setw(8) << std::hex << val;
    return Stream.str();
}