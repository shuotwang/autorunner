#ifndef COMMANDS_RESPONDER
#define COMMANDS_RESPONDER

#include <iostream>

void sendCommand(int cycle, const std::string& type, const std::string& data);

void insertFW(std::string path);
void insertCR(std::string path);

void upBtn();
void downBtn();
void leftBtn();
void rightBtn();

void uBtn();
void iBtn();
void jBtn();
void kBtn();
void cmdBtn();

void outputRegs();
void outputCSRs();
void outputMem();

#endif