#include <stdint.h>

#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H

// Graphics / Text Mode Controller
typedef struct{
    uint32_t DMode : 1;
    uint32_t DRefresh : 7;
    uint32_t DReserved : 24;
} ModeControl;

#endif