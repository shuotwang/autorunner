#include <iostream>
#include "CommandsResponder.h"

void sendCommand(int cycle, const std::string& type, const std::string& data) {

    if (type == "InsertFW") {
        insertFW(data);
    }else if (type == "InsertCart") {
        insertCR(data);
    }else if (type == "DirectionUp"){
        upBtn();
    }else if (type == "DirectionDown") {
        downBtn();
    }else if (type == "DirectionLeft") {
        leftBtn();
    }else if (type == "DirectionRight") {
        rightBtn();
    }else if (type == "uBtn") {
        uBtn();
    }else if (type == "iBtn") {
        iBtn();
    }else if (type == "jBtn") {
        jBtn();
    }else if (type == "kbtn") {
        kBtn();
    }else if (type == "cmdBtn") {
        cmdBtn();
    }else if (type == "outputRegs") {
        outputRegs();
    }else if (type == "outputCSRs") {
        outputCSRs();
    }else if (type == "outputMem") {
        outputMem();
    }
}

void insertFW(std::string path){
    std::cout << "CR::InsertFW" << std::endl;
}

void insertCR(std::string path) {
    std::cout << "CR::InsertCR" << std::endl;
}

void upBtn() {
    
}

void downBtn() {

}

void leftBtn() {

}

void rightBtn() {

}

void uBtn(){

}
void iBtn(){

}
void jBtn(){

}
void kBtn(){

}
void cmdBtn(){

}
void outputRegs(){

}
void outputCSRs(){

}
void outputMem(){

}