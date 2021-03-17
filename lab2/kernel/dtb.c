#include "dtb.h"

extern uint64_t DTB_ADDR;

static int dtb_node_name_strcmp(const char *find, const char *from){
    while(*find){
        if (*find != *from){
            break;
        }
        find++;
        from++;
    }
    if(*find == '\0' && *from == '@'){
        return *(const unsigned char*)(find - 1) - *(const unsigned char*)(from - 1);
    }
    return *(const unsigned char*)find - *(const unsigned char*)from;
}

static void dtb_print_raw_token(const uint32_t *token){
    char buffer[10];
    uitoa(bswap32(*token), buffer, sizeof(buffer), 16);
    miniuart_send_S("0x");
    miniuart_send_S(buffer);
}

static uint32_t dtb_print_node_name(const char *str, uint32_t printing){
    uint32_t len = 0;
    while(1){
        if(str[len] == '\0'){
            if(printing && !len){  //root node
                miniuart_send_C('/');
            }
            return len + 1;
        }
        if(printing){
            miniuart_send_C(str[len]);
        }
        len++;
    }
}

static void dtb_print_string_list(const char *str, uint32_t len){
    miniuart_send_C('\"');
    for(uint32_t i=1; i<len; i++){
        if(str[i-1] == '\0'){
            if(str[i-2] != '\0'){
                miniuart_send_S("\", \"");
            }
        }else{
            miniuart_send_C(str[i-1]);
        }
    }
    miniuart_send_S("\"");
}

static void dtb_print_prop(const char *name, const uint32_t *value, uint32_t len){
    if(!strcmp(name, "serial-number") ||
        !strcmp(name, "compatible") ||
        !strcmp(name, "model") ||
        !strcmp(name, "status") ||
        !strcmp(name, "bootargs") ||
        !strcmp(name, "pinctrl-names") ||
        !strcmp(name, "clock-names") ||
        !strcmp(name, "dma-names") ||
        !strcmp(name, "clock-output-names") ||
        !strcmp(name, "phy-names") ||
        !strcmp(name, "interrupt-names") ||
        !strcmp(name, "reg-names") ||
        !strcmp(name, "enable-method") ||
        !strcmp(name, "device_type") ||
        !strcmp(name, "label") ||
        !strcmp(name, "default-state") ||
        !strcmp(name, "linux,default-trigger") ||
        !strcmp(name, "regulator-name")
    ){
        dtb_print_string_list((char *)value, len);
        miniuart_send_C(';');
    }else{
        uint32_t ntoken = ROUNDUP_MUL4(len) >> 2;
        while(ntoken--){
            dtb_print_raw_token(value);
            miniuart_send_S("  ");
            value++;
        }
    }
}

static void dtb_traverse_node(const uint32_t *token, const char *str_blk, const char *find_node){
    uint32_t space = 0;
    char *node_name;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            if(space){
                miniuart_send_S(NEW_LINE);
                miniuart_send_nC(' ', space);
                node_name = (char *)token;
                token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
                miniuart_send_S(" {" NEW_LINE);
                space += 4;
            }else{
                if(!dtb_node_name_strcmp(find_node, (char *)token)){
                    space += 4;
                    node_name = (char *)token;
                    token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
                    miniuart_send_S(" {" NEW_LINE);
                }else{
                    token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
                }
            }
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            if(space){
                space -= 4;
                miniuart_send_nC(' ', space);
                miniuart_send_S("};" NEW_LINE);
            }
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else if(bswap32(*token) == 0x3){  //property
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            if(space){
                miniuart_send_nC(' ', space);
                miniuart_send_S(str_blk + bswap32(prop_desc->nameoff));
                miniuart_send_S(" = ");
                if(!strcmp(node_name, "aliases") ||
                    !strcmp(node_name, "__symbols__")
                ){
                    dtb_print_string_list((char *)token, bswap32(prop_desc->len));
                    miniuart_send_C(';');
                }else if(!strcmp(node_name, "__overrides__")){
                    dtb_print_raw_token(token);
                    if(bswap32(prop_desc->len) > 4){
                        miniuart_send_S("  ");
                        dtb_print_string_list((char *)(token + 1), bswap32(prop_desc->len) - 4);
                    }
                }else{
                    dtb_print_prop(str_blk + bswap32(prop_desc->nameoff), token, bswap32(prop_desc->len));
                }
                miniuart_send_S(NEW_LINE);
            }
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else{
            token++;
            miniuart_send_S("Error: Unknown FDT node!!!  " );
            dtb_print_raw_token(token);
            miniuart_send_S(NEW_LINE);
        }
    }
}

void dtb_parse(char *find_node){
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) != DTB_MAGIC_NUM){
        miniuart_send_S("Error: Invalid dtb Format!!!" NEW_LINE);
        return;
    }
    dtb_traverse_node((uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct)), (char *)DTB_ADDR + bswap32(blk->off_dt_strings), find_node);
}
