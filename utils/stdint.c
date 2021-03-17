typedef unsigned long long int uint64_t;
typedef unsigned char uint8_t;

uint8_t hex_to_int8(char hex){
    if(hex >= '0' && hex <= '9')
        return hex-'0';
    else if(hex >= 'A' && hex <= 'Z')
        return hex-'A'+10;
    else if(hex >= 'a' && hex <= 'z')
        return hex-'a'+10;
    else
        return -1;
}

uint64_t hex_to_int64(char* num){
    uint64_t res=0;
    for(int i=0; i<8; i++){
        res = (res<<4) + hex_to_int8(num[i]);
    }
    return res;
}