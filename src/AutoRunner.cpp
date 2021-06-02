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

const std::string CAutoRunner::INIT_STRING = "Init";
const std::string CAutoRunner::TIMER_US_STRING = "TimerUS";
const std::string CAutoRunner::VIDEO_MS_STRING = "VideoMS";
const std::string CAutoRunner::CPU_FREQ_STRING = "CPUFreq";
const std::string CAutoRunner::COMMANDS_STRING = "Commands";
const std::string CAutoRunner::CYCLE_STRING = "Cycle";
const std::string CAutoRunner::TYPE_STRING = "Type";
const std::string CAutoRunner::DATA_STRING = "Data";
const std::string CAutoRunner::DIRECTION_UP_STRING = "DirectionUp";
const std::string CAutoRunner::DIRECTION_DOWN_STRING = "DirectionDown";
const std::string CAutoRunner::DIRECTION_LEFT_STRING = "DirectionLeft";
const std::string CAutoRunner::DIRECTION_RIGHT_STRING = "DirectionRight";
const std::string CAutoRunner::U_BUTTON_STRING = "UBtn";
const std::string CAutoRunner::I_BUTTON_STRING = "IBtn";
const std::string CAutoRunner::J_BUTTON_STRING = "JBtn";
const std::string CAutoRunner::K_BUTTON_STRING = "KBtn";
const std::string CAutoRunner::DIRECTION_UP_RELEASE_STRING = "DirectionUpRelease";
const std::string CAutoRunner::DIRECTION_DOWN_RELEASE_STRING = "DirectionDownRelease";
const std::string CAutoRunner::DIRECTION_LEFT_RELEASE_STRING = "DirectionLeftRelease";
const std::string CAutoRunner::DIRECTION_RIGHT_RELEASE_STRING = "DirectionRightRelease";
const std::string CAutoRunner::U_BUTTON_RELEASE_STRING = "UBtnRelease";
const std::string CAutoRunner::I_BUTTON_RELEASE_STRING = "IBtnRelease";
const std::string CAutoRunner::J_BUTTON_RELEASE_STRING = "JBtnRelease";
const std::string CAutoRunner::K_BUTTON_RELEASE_STRING = "KBtnRelease";
const std::string CAutoRunner::INSERT_FW_STRING = "InsertFW";
const std::string CAutoRunner::INSERT_CR_STRING = "InsertCart";
const std::string CAutoRunner::REMOVE_CR_STRING = "RemoveCart";
const std::string CAutoRunner::CMD_BUTTON_STRING = "CMDBtn"; 
const std::string CAutoRunner::OUTPUT_REG_STRING = "OutputRegs";
const std::string CAutoRunner::OUTPUT_CSR_STRING = "OutputCSRs";
const std::string CAutoRunner::OUTPUT_MEM_STRING = "OutputMem";


CAutoRunner::CAutoRunner(int argc, char *argv[]){
    if (DJSONFilePath.back() == '/') {
        DInputJSONPath = DJSONFilePath + "input.json";
        DOutputJSONPath = DJSONFilePath + "output.json";
    }else {
        DInputJSONPath = DJSONFilePath + "/input.json";
        DOutputJSONPath = DJSONFilePath + "/output.json";
    }

    ParseArguments(argc, argv);
    DInputJSONDocument = GetInputJSONDocument();
    ParseInitData();

    DOutputJSONDocument.SetObject();
    rapidjson::Value TempValue(rapidjson::kArrayType);
    DOutputJSONObjectArray = TempValue;

    ParseCommandData();
    OutputJSONFile();
}

void CAutoRunner::ParseArguments(int &argc, char *argv[]){
    if (argc >= 2) {
        DInputJSONPath = argv[1];
        if (argc >= 3) {
            DOutputJSONPath = argv[2];
        }
    }
}

rapidjson::Document CAutoRunner::GetInputJSONDocument() {
    FILE* fp = fopen(DInputJSONPath.c_str(), "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    return d;
}

void CAutoRunner::ParseInitData() {
    uint32_t TimerUS = 0, VideoMS = 0, CPUFreq = 0;

    if (DInputJSONDocument.HasMember(INIT_STRING.c_str())) {
        const rapidjson::Value& Init = DInputJSONDocument[INIT_STRING.c_str()];
        if (Init[TIMER_US_STRING.c_str()].IsInt()) {
            TimerUS = Init[TIMER_US_STRING.c_str()].GetInt(); 
        }

        if (Init[VIDEO_MS_STRING.c_str()].IsInt()) {
            VideoMS = Init[VIDEO_MS_STRING.c_str()].GetInt();
        }

        if (Init[CPU_FREQ_STRING.c_str()].IsInt()) {
            CPUFreq = Init[CPU_FREQ_STRING.c_str()].GetInt();
        }
    }
    DRISCVConsole = std::make_shared<CRISCVConsole>(TimerUS, VideoMS, CPUFreq);
    DRISCVConsole->SetDebugMode(true);
}

void CAutoRunner::ParseCommandData() {
    if (DInputJSONDocument.HasMember(COMMANDS_STRING.c_str())) {
        const rapidjson::Value& Commands = DInputJSONDocument[COMMANDS_STRING.c_str()];
        if (Commands.IsArray()) {
            size_t len = Commands.Size();
            for (size_t i = 0; i < len; i++) {
                uint32_t CurrentCycle = 0;
                uint32_t NextCycle = 0;
                std::string Type = "";
                std::string Data = "";

                const rapidjson::Value& CMD = Commands[i];

                if (CMD.HasMember(CYCLE_STRING.c_str()) && CMD[CYCLE_STRING.c_str()].IsUint()) {
                    CurrentCycle = CMD[CYCLE_STRING.c_str()].GetUint();
                }

                if (CMD.HasMember(TYPE_STRING.c_str()) && CMD[TYPE_STRING.c_str()].IsString()) {
                    Type = CMD[TYPE_STRING.c_str()].GetString();
                }

                if (CMD.HasMember(DATA_STRING.c_str()) && CMD[DATA_STRING.c_str()].IsString()) {
                    Data = CMD[DATA_STRING.c_str()].GetString();
                    Data = Data == "" ? "." : Data;
                }
                
                if (i + 1 < len) {
                    const rapidjson::Value &NextCMD = Commands[i+1];
                    if (NextCMD.HasMember(CYCLE_STRING.c_str()) && NextCMD[CYCLE_STRING.c_str()].IsUint()) {
                        NextCycle = NextCMD[CYCLE_STRING.c_str()].GetUint();
                    }
                }else {
                    NextCycle = CurrentCycle;
                }
                SendCommand(CurrentCycle, NextCycle, Type, Data);                    
            }
        }
    }
}

bool CAutoRunner::IsDirectionButton(std::string &type) {
    if (type == DIRECTION_UP_STRING || type == DIRECTION_DOWN_STRING || \
        type == DIRECTION_LEFT_STRING || type == DIRECTION_RIGHT_STRING) {
            return true;
        }
    return false;
}

bool CAutoRunner::IsNumberButton(std::string &type) {
    if (type == U_BUTTON_STRING || type == I_BUTTON_STRING || \
        type == J_BUTTON_STRING || type == K_BUTTON_STRING){
        return true;
    }
    return false;
}

bool CAutoRunner::IsDirectionReleaseButton(std::string &type) {
    if (type == DIRECTION_UP_RELEASE_STRING || type == DIRECTION_DOWN_RELEASE_STRING || \
        type == DIRECTION_LEFT_RELEASE_STRING || type == DIRECTION_RIGHT_RELEASE_STRING) {
            return true;
        }
    return false;
}

bool CAutoRunner::IsNumberReleaseButton(std::string &type) {
    if (type == U_BUTTON_RELEASE_STRING || type == I_BUTTON_RELEASE_STRING || \
        type == J_BUTTON_RELEASE_STRING || type == K_BUTTON_RELEASE_STRING){
        return true;
    }
    return false;
}
    
void CAutoRunner::OutputJSONFile() {
    rapidjson::Document::AllocatorType &OutputAllocator = DOutputJSONDocument.GetAllocator();
    DOutputJSONDocument.AddMember("Outputs", DOutputJSONObjectArray, OutputAllocator);
    FILE* f = fopen(DOutputJSONPath.c_str(), "w");
	char WriteBuffer[65535];
	rapidjson::FileWriteStream os(f, WriteBuffer, sizeof(WriteBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> Writer(os);
	DOutputJSONDocument.Accept(Writer);
	fclose(f);
}

void CAutoRunner::SendCommand(uint32_t cycle, uint32_t nextCycle, std::string &type, std::string &data) {
    if (type == INSERT_FW_STRING) {
        InsertFW(data);
        DoPowerOn();
    }else if (type == INSERT_CR_STRING) {
        InsertCR(data);
    }else if (type == REMOVE_CR_STRING) {
        RemoveCR();
    }else if (IsDirectionButton(type)){
        PressDirection(type);
    }else if (IsNumberButton(type)) {
        PressButton(type);
    }else if (IsDirectionReleaseButton(type)) {
        ReleaseDirection(type);
    }else if (IsNumberReleaseButton(type)){
        ReleaseButton(type);  
    }else if (type == CMD_BUTTON_STRING) {
        PressCommand();
    }else if (type == OUTPUT_REG_STRING) {
        std::map<std::string, std::string> outRegs = OutputRegs();
        rapidjson::Document::AllocatorType &OutputAllocator = DOutputJSONDocument.GetAllocator();
        rapidjson::Value outRegJsonVal = FormatOutputMap(outRegs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", cycle, OutputAllocator);
        temp.AddMember("Regs", outRegJsonVal, OutputAllocator);
        DOutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }else if (type == OUTPUT_CSR_STRING) {
        std::map<std::string, std::string> outCSRs = OutputCSRs();
        rapidjson::Document::AllocatorType &OutputAllocator = DOutputJSONDocument.GetAllocator();
        rapidjson::Value outCSRJsonVal = FormatOutputMap(outCSRs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", cycle, OutputAllocator);
        temp.AddMember("CSRs", outCSRJsonVal, OutputAllocator);
        DOutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }else if (type == OUTPUT_MEM_STRING) {
        std::map<std::string, std::string> outMem = OutputMem(data);
        rapidjson::Document::AllocatorType &OutputAllocator = DOutputJSONDocument.GetAllocator();
        rapidjson::Value outMemJsonVal = FormatOutputMap(outMem, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", cycle, OutputAllocator);
        temp.AddMember("Mem", outMemJsonVal, OutputAllocator);
        DOutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }
    DoCycleSteps(cycle, nextCycle);
}


bool CAutoRunner::InsertFW(std::string &data){
    if (!data.empty()){
        std::string FWFileName = data;

        auto InFile = std::make_shared<CFileDataSource>(FWFileName);
        if(DRISCVConsole->ProgramFirmware(InFile)){
            return true;
        }
    }
    return false;
}

bool CAutoRunner::InsertCR(std::string &data){
    if (!data.empty()){
        std::string CRFileName = data;

        auto InFile = std::make_shared<CFileDataSource>(CRFileName);
        if(DRISCVConsole->InsertCartridge(InFile)){
            return true;
        }
    }
    return false;
}

bool CAutoRunner::RemoveCR() {
    DRISCVConsole->RemoveCartridge();
    return true;
}

bool CAutoRunner::PressDirection(std::string &type){
    if (!type.empty()) {
        if (type == "DirectionUp") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Up);
            return true;
        } else if (type == "DirectionDown") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Down);
            return true;
        } else if (type == "DirectionLeft") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Left);
            return true;
        } else if (type == "DirectionRight") {
            DRISCVConsole->PressDirection(CRISCVConsole::EDirection::Right);
            return true;
        }
    }
    return false;
}

bool CAutoRunner::ReleaseDirection(std::string &type) {
    if (!type.empty()) {
        if (type == "DirectionUp") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Up);
            return true;
        } else if (type == "DirectionDown") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Down);
            return true;
        } else if (type == "DirectionLeft") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Left);
            return true;
        } else if (type == "DirectionRight") {
            DRISCVConsole->ReleaseDirection(CRISCVConsole::EDirection::Right);
            return true;
        }
    }
    return false;
}

bool CAutoRunner::PressButton(std::string &type) {
    if (!type.empty()) {
        if (type == "UBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button1);
            return true;
        }else if (type == "IBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button2);
            return true;
        }else if (type == "JBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button3);
            return true;
        }else if (type == "KBtn") {
            DRISCVConsole->PressButton(CRISCVConsole::EButtonNumber::Button4);
            return true;
        }
    }
    return false;
}

bool CAutoRunner::ReleaseButton(std::string &type) {
    if (!type.empty()) {
        if (type == "UBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button1);
            return true;
        }else if (type == "IBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button2);
            return true;
        }else if (type == "JBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button3);
            return true;
        }else if (type == "KBtn") {
            DRISCVConsole->ReleaseButton(CRISCVConsole::EButtonNumber::Button4);
            return true;
        }
    }
    return false;
}

bool CAutoRunner::PressCommand() {
    DRISCVConsole->PressCommand();
    return true;
}

std::map<std::string, std::string> CAutoRunner::OutputRegs(){
    std::map<std::string, std::string> RegOutputMap;

    for(size_t Index = 0; Index < CRISCVCPU::RegisterCount(); Index++){
        auto RegisterName = CRISCVCPU::CInstruction::RegisterName(Index);
        RegOutputMap[RegisterName] = FormatHex32Bit(DRISCVConsole->CPU()->Register(Index));
    }

    return RegOutputMap;
}

std::map<std::string, std::string> CAutoRunner::OutputCSRs(){
    std::map<std::string, std::string> CSROutputMap;
    size_t LineIndex = 0;
    for(uint32_t CSRAddr : DRISCVConsole->CPU()->ControlStatusRegisterKeys()){
        std::string CSRName = CRISCVSYSTypeInstruction::ControlStatusRegisterName(CSRAddr);
        CSROutputMap[CSRName] = FormatHex32Bit(DRISCVConsole->CPU()->ControlStatusRegister(CSRAddr));
        LineIndex++;
    }
    return CSROutputMap;
}

std::map<std::string, std::string> CAutoRunner::OutputMem(std::string &data){
    // reference from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    std::string Delim = "-";

    auto Start = 0U;
    auto End = data.find(Delim);
    std::string StartAdd;
    std::string EndAdd;
    
    while (End != std::string::npos)
    {
        StartAdd = data.substr(Start, End - Start);
        Start = End + Delim.length();
        End = data.find(Delim, Start);
    }

    EndAdd = data.substr(Start, End);

    uint32_t StartAddHex = GetAddHex(StartAdd);
    uint32_t Bytes = GetAddHex(EndAdd) - StartAddHex + 1;

    std::stringstream OutputStream;
    const uint8_t *Buffer = DRISCVConsole->Memory()->LoadData(StartAddHex, Bytes);

    for(uint32_t Index = 0; Index < Bytes; Index++){
        OutputStream<<std::setfill('0') << std::setw(2) << std::hex << uint32_t(Buffer[Index]);
    }
    return {{StartAdd, OutputStream.str()}};
}

bool CAutoRunner::DoStep() {
    DRISCVConsole->Step();
    return true;
}

bool CAutoRunner::DoRun() {
    DRISCVConsole->Run();
    return true;
}

bool CAutoRunner::DoStop() {
    DRISCVConsole->Stop();
    return true;
}

bool CAutoRunner::DoPowerOn() {
    DRISCVConsole->PowerOn();
    return true;
}

bool CAutoRunner::DoPowerOff() {
    DRISCVConsole->PowerOff();
    return true;
}

bool CAutoRunner::DoCycleSteps(uint32_t cycle, uint32_t nextCycle){
    for (uint32_t i=0; i < (nextCycle - cycle); i++) {
        DoStep();
    }
    return true;
}   

uint32_t CAutoRunner::GetAddHex(std::string strAdd){
    // reference from: https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << strAdd;
    ss >> x;
    // output it as a signed type
    uint32_t addr = static_cast<int>(x);
    return addr;
}

rapidjson::Value CAutoRunner::FormatOutputMap(std::map<std::string, std::string> map, rapidjson::Document::AllocatorType &allocator){
    rapidjson::Value root(rapidjson::kObjectType);
    rapidjson::Value key(rapidjson::kStringType);  
    rapidjson::Value value(rapidjson::kStringType); 
 
	for(std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it){
		key.SetString(it->first.c_str(), allocator);  
   		value.SetString(it->second.c_str(), allocator);  
    	root.AddMember(key, value, allocator);
	}

    return root;
}

std::string CAutoRunner::FormatHex32Bit(uint32_t val){
    std::stringstream Stream;
    Stream<<std::setfill('0') << std::setw(8) << std::hex << val;
    return Stream.str();
}