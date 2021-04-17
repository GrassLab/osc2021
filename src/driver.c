#include "driver.h"

void bcm2835_gpio_probe(char* node_addr, struct fdt_header* header, int depth){
      __show_device_node_info(node_addr,  header, depth);
}
void RTX_3080_probe(char* node_addr, struct fdt_header* header, int depth){
    __show_device_node_info(node_addr,  header, depth);
}

void arm_cortex_a53_probe(char* node_addr, struct fdt_header* header, int depth){
    __show_device_node_info(node_addr,  header, depth);
}
void ALL_probe(char* node_addr, struct fdt_header* header, int depth){
    __show_device_node_info(node_addr,  header, depth);
}