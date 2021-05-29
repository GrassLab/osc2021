#pragma once

/**
 * ref:
 *  statement expression
 *  https://stackoverflow.com/questions/3437404/min-and-max-in-c
 *  https: // gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs
 */

#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })
