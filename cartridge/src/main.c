#include <stdint.h>
#include <stdio.h>
#include "sys_call.h"

/* THIS IS CARTRIDGE */

#define CONTROLLER      (*((volatile uint32_t *)0x40000018))
volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

int main() {
    
    // Draw small palette
    // Initialize Palette 0 and 1
    // TODO: change to system calls
    (*((volatile uint32_t *)0x500FD000)) = 0xFFFF0000;
    (*((volatile uint32_t *)0x500FD004)) = 0xFF008000;

    // Initialize Palette control 0
    (*((volatile uint32_t *)0x500FF214)) = 0x1FE20080;

    uint32_t controllerStatus;
    uint32_t currentTicks;
    uint32_t lastGlobalTicks = 0;

    VIDEO_MEMORY[0] = 'C';
    VIDEO_MEMORY[1] = 'a';
    VIDEO_MEMORY[2] = 'r';
    VIDEO_MEMORY[3] = 't';
    VIDEO_MEMORY[4] = 'r';
    VIDEO_MEMORY[5] = 'i';
    VIDEO_MEMORY[6] = 'd';
    VIDEO_MEMORY[7] = 'g';
    VIDEO_MEMORY[8] = 'e';
    VIDEO_MEMORY[9] = ' ';
    VIDEO_MEMORY[10] = 'T';
    VIDEO_MEMORY[11] = 'X';
    VIDEO_MEMORY[12] = 'T';
    VIDEO_MEMORY[13] = ' ';
    VIDEO_MEMORY[14] = 'M';
    VIDEO_MEMORY[15] = 'O';
    VIDEO_MEMORY[16] = 'D';
    VIDEO_MEMORY[17] = 'E';

    while(1) {
        currentTicks = getTimerTicks();
        if(currentTicks != lastGlobalTicks){
            controllerStatus = CONTROLLER;
            if(controllerStatus){
                if(controllerStatus & 0x1){         // left
                   toGraphicsMode();
                }
                if(controllerStatus & 0x2){         // up
                   
                }
                if(controllerStatus & 0x4){         // down 
                    
                }
                if(controllerStatus & 0x8){         // right
                    toTextMode();
                }
            }
            lastGlobalTicks = currentTicks;
        }
    }
    return 0;
}
