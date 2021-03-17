#ifndef __DRIVER_H__
#define __DRIVER_H__
#include "utils.h"
#include "system.h"
#include "dtb_parser.h"

#define DRIVER_NUM 4
void bcm2835_gpio_probe(char*, struct fdt_header*, int);
void arm_cortex_a53_probe(char*, struct fdt_header*, int );
void RTX_3080_probe(char*, struct fdt_header*, int);
void ALL_probe(char*, struct fdt_header*, int);
#endif