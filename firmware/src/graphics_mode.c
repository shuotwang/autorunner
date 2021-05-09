#include <stdint.h>
#include "graphics_mode.h"
#include "typedefs.h"

/* Implementation */



void setGraphicsMode(uint32_t refresh){
    volatile ModeControl *MODE_CONTROL_REG = (volatile ModeControl *)(0x500FF414);
    MODE_CONTROL_REG->DMode = 1;
    MODE_CONTROL_REG->DRefresh = refresh;
}

