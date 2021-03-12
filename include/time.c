#include "time.h"
#include "utility.h"

float get_time () {
    return (float) time_counter() / time_freq();
}
