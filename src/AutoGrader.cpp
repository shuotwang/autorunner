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
#include "RISCVConsole.h"
#include "FileDataSink.h"
#include "FileDataSource.h"
#include "Path.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <map>

std::unordered_map<uint32_t, std::string> AutoGrader::CSRMap = 
{   // Machine Information Registers
    {0xF11,"mvendorid"},
    {0xF12,"marchid"},
    {0xF13,"mimpid"},
    {0xF14,"mhartid"},
    //Machine Trap Setup
    {0x300,"mstatus"},
    {0x301,"misa"},
    {0x302,"medeleg"},
    {0x303,"mideleg"},
    {0x304,"mie"},
    {0x305,"mtvec"},
    {0x306,"mcounteren"},
    // Machine Trap Handling
    {0x340,"mscratch"},
    {0x341,"mepc"},
    {0x342,"mcause"},
    {0x343,"mtval"},
    {0x344,"mip"},
    // Machine Memory Protection
    {0x3A0,"pmpcfg0"}, {0x3A1,"pmpcfg1"}, {0x3A2,"pmpcfg2"}, {0x3A3,"pmpcfg3"},
    {0x3B0,"pmpaddr0"}, {0x3B1,"pmpaddr1"}, {0x3B2,"pmpaddr2"}, {0x3B3,"pmpaddr3"},
    {0x3B4,"pmpaddr4"}, {0x3B5,"pmpaddr5"}, {0x3B6,"pmpaddr6"}, {0x3B7,"pmpaddr7"},
    {0x3B8,"pmpaddr8"}, {0x3B9,"pmpaddr9"}, {0x3BA,"pmpaddr10"}, {0x3BB,"pmpaddr11"},
    {0x3BC,"pmpaddr12"}, {0x3BD,"pmpaddr13"}, {0x3BE,"pmpaddr14"}, {0x3BF,"pmpaddr15"},
    // Machine Counter/Timers
    {0xB00,"mcycle"},
    {0xB02,"minstret"},
    {0xB03,"mhpmcounter3"},
    {0xB04,"mhpmcounter4"},  {0xB05,"mhpmcounter5"},  {0xB06,"mhpmcounter6"},  {0xB07,"mhpmcounter7"},
    {0xB08,"mhpmcounter8"},  {0xB09,"mhpmcounter9"},  {0xB0A,"mhpmcounter10"}, {0xB0B,"mhpmcounter11"},
    {0xB0C,"mhpmcounter12"}, {0xB0D,"mhpmcounter13"}, {0xB0E,"mhpmcounter14"}, {0xB0F,"mhpmcounter15"},
    {0xB10,"mhpmcounter16"}, {0xB11,"mhpmcounter17"}, {0xB12,"mhpmcounter18"}, {0xB13,"mhpmcounter19"},
    {0xB14,"mhpmcounter20"}, {0xB15,"mhpmcounter21"}, {0xB16,"mhpmcounter22"}, {0xB17,"mhpmcounter23"},
    {0xB18,"mhpmcounter24"}, {0xB19,"mhpmcounter25"}, {0xB1A,"mhpmcounter26"}, {0xB1B,"mhpmcounter27"},
    {0xB1C,"mhpmcounter28"}, {0xB1D,"mhpmcounter29"}, {0xB1E,"mhpmcounter30"}, {0xB1F,"mhpmcounter31"},
    {0xB80,"mcycleh"},
    {0xB82,"minstreth"},
    {0xB83,"mhpmcounter3h"},
    {0xB84,"mhpmcounter4h"},  {0xB85,"mhpmcounter5h"},  {0xB86,"mhpmcounter6h"},  {0xB87,"mhpmcounter7h"},
    {0xB88,"mhpmcounter8h"},  {0xB89,"mhpmcounter9h"},  {0xB8A,"mhpmcounter10h"}, {0xB8B,"mhpmcounter11h"},
    {0xB8C,"mhpmcounter12h"}, {0xB8D,"mhpmcounter13h"}, {0xB8E,"mhpmcounter14h"}, {0xB8F,"mhpmcounter15h"},
    {0xB90,"mhpmcounter16h"}, {0xB91,"mhpmcounter17h"}, {0xB92,"mhpmcounter18h"}, {0xB93,"mhpmcounter19h"},
    {0xB94,"mhpmcounter20h"}, {0xB95,"mhpmcounter21h"}, {0xB96,"mhpmcounter22h"}, {0xB97,"mhpmcounter23h"},
    {0xB98,"mhpmcounter24h"}, {0xB99,"mhpmcounter25h"}, {0xB9A,"mhpmcounter26h"}, {0xB9B,"mhpmcounter27h"},
    {0xB9C,"mhpmcounter28h"}, {0xB9D,"mhpmcounter29h"}, {0xB9E,"mhpmcounter30h"}, {0xB9F,"mhpmcounter31h"},
    // Machine Counter Setup
    {0x320,"mcountinhibit"},
    {0x323,"mhpmevent3"},
    {0x324,"mhpmevent4"},  {0x325,"mhpmevent5"},  {0x326,"mhpmevent6"},  {0x327,"mhpmevent7"},
    {0x328,"mhpmevent8"},  {0x329,"mhpmevent9"},  {0x32A,"mhpmevent10"}, {0x32B,"mhpmevent11"},
    {0x32C,"mhpmevent12"}, {0x32D,"mhpmevent13"}, {0x32E,"mhpmevent14"}, {0x32F,"mhpmevent15"},
    {0x330,"mhpmevent16"}, {0x331,"mhpmevent17"}, {0x332,"mhpmevent18"}, {0x333,"mhpmevent19"},
    {0x334,"mhpmevent20"}, {0x335,"mhpmevent21"}, {0x336,"mhpmevent22"}, {0x337,"mhpmevent23"},
    {0x338,"mhpmevent24"}, {0x339,"mhpmevent25"}, {0x33A,"mhpmevent26"}, {0x33B,"mhpmevent27"},
    {0x33C,"mhpmevent28"}, {0x33D,"mhpmevent29"}, {0x33E,"mhpmevent30"}, {0x33F,"mhpmevent31"},

};

AutoGrader::AutoGrader(char *InputJSONPath, char *OutputJSONPath){
    AutoGrader::InputJSONPath = InputJSONPath;
    AutoGrader::OutputJSONPath = OutputJSONPath;

    InputJSONDocument = GetInputJSONDocument(InputJSONPath);
    ParseInitData();

    OutputJSONDocument.SetObject();

    rapidjson::Value TempValue(rapidjson::kArrayType);
    OutputJSONObjectArray = TempValue;

    // Run
    ParseCommandData();
    OutputJSONFile();
}

rapidjson::Document AutoGrader::GetInputJSONDocument(char *InputJSONPath) {
    FILE* fp = fopen(InputJSONPath, "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    return d;
}

void AutoGrader::ParseInitData() {
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
                        DoStep();
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
    



void AutoGrader::OutputJSONFile() {
    rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
    OutputJSONDocument.AddMember("Outputs", OutputJSONObjectArray, OutputAllocator);
    FILE* f = fopen(OutputJSONPath, "w");
	char writeBuffer[65535];
	rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	OutputJSONDocument.Accept(writer);
	fclose(f);
}

void AutoGrader::SendCommand(uint32_t Cycle, std::string &Type, std::string &Data) {
    if (Type == "InsertFW") {
        InsertFW(Data);
        DoPowerOn();
    }else if (Type == "InsertCart") {
        InsertCR(Data);
    }else if (Type == "RemoveCart") {
        RemoveCR();
    }else if (Type == "DirectionUp" || Type == "DirectionDown" || \
                Type == "DirectionLeft" || Type == "DirectionRight"){
        PressDirection(Type);
    }else if (Type == "UBtn" || Type == "IBtn" || Type == "JBtn" || Type == "KBtn") {
        PressButton(Type);
    }else if (Type == "CMDBtn") {
        PressCommand();
    }else if (Type == "OutputRegs") {
        std::map<std::string, std::string> outRegs = OutputRegs();
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outRegJsonVal = FormatOutputMap(outRegs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("Regs", outRegJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }else if (Type == "OutputCSRs") {
        std::map<std::string, std::string> outCSRs = OutputCSRs();
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outCSRJsonVal = FormatOutputMap(outCSRs, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("CSRs", outCSRJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }else if (Type == "OutputMem") {
        std::map<std::string, std::string> outMem = OutputMem(Data);
        rapidjson::Document::AllocatorType &OutputAllocator = OutputJSONDocument.GetAllocator();
        rapidjson::Value outMemJsonVal = FormatOutputMap(outMem, OutputAllocator);
        rapidjson::Value temp(rapidjson::kObjectType);
        temp.AddMember("Cycle", Cycle, OutputAllocator);
        temp.AddMember("Mem", outMemJsonVal, OutputAllocator);
        OutputJSONObjectArray.PushBack(temp, OutputAllocator);
    }
}


bool AutoGrader::InsertFW(std::string &Data){
    if (!Data.empty()){
        std::string FWFileName = Data;

        auto InFile = std::make_shared<CFileDataSource>(FWFileName);
        if(DRISCVConsole->ProgramFirmware(InFile)){
            std::cout << "FW Loaded" << std::endl;
            return true;
        }
    }
    return false;
}

bool AutoGrader::InsertCR(std::string &Data){
    if (!Data.empty()){
        std::string CRFileName = Data;

        auto InFile = std::make_shared<CFileDataSource>(CRFileName);
        if(DRISCVConsole->InsertCartridge(InFile)){
            std::cout << "CR Loaded" << std::endl;
            return true;
        }
    }
    return false;
}

bool AutoGrader::RemoveCR() {
    DRISCVConsole->RemoveCartridge();
    return true;
}

bool AutoGrader::PressDirection(std::string &Data){
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

bool AutoGrader::ReleaseDirection(std::string &Data) {
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

bool AutoGrader::PressButton(std::string &Data) {
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

bool AutoGrader::ReleaseButton(std::string &Data) {
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

bool AutoGrader::PressCommand() {
    DRISCVConsole->PressCommand();
    return true;
}

std::map<std::string, std::string> AutoGrader::OutputRegs(){
    std::map<std::string, std::string> RegOutputMap;

    for(size_t Index = 0; Index < CRISCVCPU::RegisterCount(); Index++){
        auto RegisterName = CRISCVCPU::CInstruction::RegisterName(Index);
        RegOutputMap[RegisterName] = FormatHex32Bit(DRISCVConsole->CPU()->Register(Index));
    }

    return RegOutputMap;
}

std::map<std::string, std::string> AutoGrader::OutputCSRs(){
    std::map<std::string, std::string> CSROutputMap;
    size_t LineIndex = 0;
    for(uint32_t CSRAddr : DRISCVConsole->CPU()->ControlStatusRegisterKeys()){
        std::string CSRName = CSRMap.find(CSRAddr)->second;
        CSROutputMap[CSRName] = FormatHex32Bit(DRISCVConsole->CPU()->ControlStatusRegister(CSRAddr));
        LineIndex++;
    }
    return CSROutputMap;
}

std::map<std::string, std::string> AutoGrader::OutputMem(std::string &Data){
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

bool AutoGrader::DoStep() {
    DRISCVConsole->Step();
    return true;
}

bool AutoGrader::DoRun() {
    DRISCVConsole->Run();
    return true;
}

bool AutoGrader::DoStop() {
    DRISCVConsole->Stop();
    return true;
}

bool AutoGrader::DoPowerOn() {
    DRISCVConsole->PowerOn();
    return true;
}

bool AutoGrader::DoPowerOff() {
    DRISCVConsole->PowerOff();
    return true;
}

uint32_t AutoGrader::GetAddHex(std::string strAdd){
    // reference from: https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << strAdd;
    ss >> x;
    // output it as a signed type
    uint32_t addr = static_cast<int>(x);
    return addr;
}

rapidjson::Value AutoGrader::FormatOutputMap(std::map<std::string, std::string> Map, rapidjson::Document::AllocatorType &Allocator){
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

std::string AutoGrader::FormatHex32Bit(uint32_t val){
    std::stringstream Stream;
    Stream<<std::setfill('0') << std::setw(8) << std::hex << val;
    return Stream.str();
}