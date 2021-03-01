#include "system.h"

void reset(int tick){
    register unsigned int r;
    //r = *PM_RSTS;
    //r &= ~0xFFFFFaaa;
    *PM_RSTC = PM_PASSWORD | 0x20;
    //*PM_RSTS = PM_PASSWORD | r;
    *PM_WDOG = PM_PASSWORD | 10;
}
void cancel_reset(){
    *PM_RSTC = PM_PASSWORD | 0x00;
    *PM_WDOG = PM_PASSWORD | 0x00;

}
