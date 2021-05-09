#include <stdint.h>

extern volatile uint32_t TimerTicks;
#define CARTRIDGE_STATUS_REG (*(volatile uint32_t *)0x4000001C)
typedef void (*TEntryFunction)(void);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

int main() {

    VIDEO_MEMORY[0] = 'F';
    VIDEO_MEMORY[1] = 'i';
    VIDEO_MEMORY[2] = 'r';
    VIDEO_MEMORY[3] = 'm';
    VIDEO_MEMORY[4] = 'w';
    VIDEO_MEMORY[5] = 'a';
    VIDEO_MEMORY[6] = 'r';
    VIDEO_MEMORY[7] = 'e';
    VIDEO_MEMORY[8] = ' ';
    VIDEO_MEMORY[9] = 'T';
    VIDEO_MEMORY[10] = 'E';
    VIDEO_MEMORY[11] = 'X';
    VIDEO_MEMORY[12] = 'T';
    VIDEO_MEMORY[13] = ' ';
    VIDEO_MEMORY[14] = 'M';
    VIDEO_MEMORY[15] = 'O';
    VIDEO_MEMORY[16] = 'D';
    VIDEO_MEMORY[17] = 'E';

    TEntryFunction EntryFunction;
    while(1){
        if(CARTRIDGE_STATUS_REG & 0x1){
            EntryFunction = (TEntryFunction)(CARTRIDGE_STATUS_REG & 0xFFFFFFFC);
            EntryFunction();
        }
    }


    return 0;
}
