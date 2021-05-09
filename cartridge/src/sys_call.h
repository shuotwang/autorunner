#include <stdint.h>

#ifndef _SYSCALL_H
#define _SYSCALL_H

/*  Declare */
// Overall system call function
uint32_t SystemCall(uint32_t param1, uint32_t param2);

// Detailed system call function
// call type 0
void toTextMode();
void toGraphicsMode();

/* Call Type 1 */
/* Controller Response */


// call type 3
uint32_t getTimerTicks();

#endif