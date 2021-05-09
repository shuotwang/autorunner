#include <stdint.h>
#include "text_mode.h"
#include "typedefs.h"

/* Implementation */


void setTextMode(uint32_t refresh){
    volatile ModeControl *MODE_CONTROL_REG = (volatile ModeControl *)(0x500FF414);
    MODE_CONTROL_REG->DMode = 0;
    MODE_CONTROL_REG->DRefresh = refresh;
}

