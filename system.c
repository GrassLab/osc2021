#include "system.h"

void reset(int tick){
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 10;
}
void cancel_reset(){
    *PM_RSTC = PM_PASSWORD | 0x00;
    *PM_WDOG = PM_PASSWORD | 0x00;

}
