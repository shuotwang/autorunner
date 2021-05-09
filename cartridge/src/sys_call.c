#include <stdint.h>

/* Implement */
// SystemCall function implemented in crt0.s

/* Call Type 0 */
// Switch mode, 0 for text mode
void toTextMode(){
    SystemCall(0, 0);
}

// Switch mode, 1 for graphic mode
void toGraphicsMode(){
    SystemCall(0, 1);
}

// TODO:
/* Call Type 1 */
/* Controller Response */
/*  0   -   Up
    1   -   Down
    2   -   Left
    3   -   Right
*/




/* Call Type 3 */
// Get timer ticks
uint32_t getTimerTicks(){
    return SystemCall(3, 0);
}

