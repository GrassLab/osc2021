#include "dtb.h"

extern uint64_t DTB_ADDR;

static int32_t dtb_node_name_strcmp(const char *find, const char *from){
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
            miniuart_send_S("Error: Unknown FDT node!!!  " );
            dtb_print_raw_token(token);
            miniuart_send_S(NEW_LINE);
            token++;
        }
    }
}

void dtb_dump(const char *find_node){
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) != DTB_MAGIC_NUM){
        miniuart_send_S("Error: Invalid dtb Format!!!" NEW_LINE);
        return;
    }
    dtb_traverse_node((uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct)), (char *)DTB_ADDR + bswap32(blk->off_dt_strings), find_node);
}

static uint32_t dtb_is_valid_node(const uint32_t *node){
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) != DTB_MAGIC_NUM){
        miniuart_send_S("Error: Invalid dtb Format!!!" NEW_LINE);
        return 0;
    }
    uint32_t *struct_blk = (uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));
    uint32_t *struct_blk_end = (uint32_t *)((char *)struct_blk + bswap32(blk->size_dt_struct));

    if(((uint64_t)node - DTB_ADDR) & 0x3){
        miniuart_send_S("Error: Invalid dtb node address!!!" NEW_LINE);
        return 0;
    }
    if((node < struct_blk) || (node > struct_blk_end)){
        miniuart_send_S("Error: Invalid dtb node address!!!" NEW_LINE);
        return 0;
    }
    return 1;
}

const uint32_t *dtb_get_node_prop_addr(const char *node_name, const uint32_t *starting_token){
    if(!starting_token){
        fdt_header *blk = (fdt_header *)DTB_ADDR;
        if(bswap32(blk->magic) != DTB_MAGIC_NUM){
            miniuart_send_S("Error: Invalid dtb Format!!!" NEW_LINE);
            return NULL;
        }
        starting_token = (uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));
    }else{
        if(!dtb_is_valid_node(starting_token)){
            return NULL;
        }
        while(bswap32(*starting_token) != 0x2 && bswap32(*starting_token) != 0x9){  //end node or end tree
            if(bswap32(*starting_token) == 0x3){
                starting_token++;
                fdt_prop_entry *prop_desc = (fdt_prop_entry *)starting_token;
                starting_token += sizeof(fdt_prop_entry) >> 2;
                starting_token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
            }else{
                starting_token++;
            }
        }
    }

    uint32_t found = 0;
    while(1){
        if(bswap32(*starting_token) == 0x1){  //begin node
            starting_token++;
            if(!dtb_node_name_strcmp(node_name, (char *)starting_token)){
                found = 1;
            }
            starting_token += ROUNDUP_MUL4(dtb_print_node_name((char *)starting_token, 0)) >> 2;
        }else if(bswap32(*starting_token) == 0x3){  //property
            if(found){
                return starting_token;
            }
            starting_token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)starting_token;
            starting_token += sizeof(fdt_prop_entry) >> 2;
            starting_token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*starting_token) == 0x9){  //end tree
            return NULL;
        }else{
            starting_token++;
        }
    }
}

int32_t dtb_get_prop_value(const char *prop_name, const uint32_t *prop_addr, void *data){
    if(!dtb_is_valid_node(prop_addr)){
        return -1;
    }
    char *str_blk = (char *)DTB_ADDR + bswap32(((fdt_header *)DTB_ADDR)->off_dt_strings);

    while(bswap32(*prop_addr) != 0x2 && bswap32(*prop_addr) != 0x9){  //end node or end tree
        prop_addr++;
        fdt_prop_entry *prop_desc = (fdt_prop_entry *)prop_addr;
        prop_addr += sizeof(fdt_prop_entry) >> 2;
        if(!strcmp(str_blk + bswap32(prop_desc->nameoff), prop_name)){
            strncpy(data, (char *)prop_addr, bswap32(prop_desc->len));
            return bswap32(prop_desc->len);
        }
        prop_addr += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
    }
    return -1;
}

static int32_t dtb_get_node_depth(const uint32_t *node){
    if(!dtb_is_valid_node(node)){
        return -1;
    }
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    uint32_t *token = (uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));

    int32_t depth = 0;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            depth++;
            token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, 0)) >> 2;
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            depth--;
        }else if(bswap32(*token) == 0x3){  //property
            if(token == node){
                return depth;
            }
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else{
            token++;
        }
    }
    return -1;
}

int32_t dtb_get_parent_prop_value(const char *prop_name, const uint32_t *child, void *data){
    if(!dtb_is_valid_node(child)){
        return -1;
    }

    int32_t node_depth = dtb_get_node_depth(child);
    if(node_depth == -1){
        return -1;
    }

    fdt_header *blk = (fdt_header *)DTB_ADDR;
    char *str_blk = (char *)DTB_ADDR + bswap32(blk->off_dt_strings);
    uint32_t *token = (uint32_t *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));

    int32_t len, depth = 0;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            depth++;
            token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, 0)) >> 2;
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            depth--;
        }else if(bswap32(*token) == 0x3){  //property
            if(token == child){
                return len;
            }
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            if(depth == node_depth - 1){
                if(!strcmp(str_blk + bswap32(prop_desc->nameoff), prop_name)){
                    strncpy(data, (char *)token, bswap32(prop_desc->len));
                    len = bswap32(prop_desc->len);
                }
            }
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else{
            token++;
        }
    }
    return -1;
}
