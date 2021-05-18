unsigned long pow(unsigned long base, int n) {
    unsigned long res = 1;
    if (!n) return res;    
    if (base == 2) {
        return res << n;
    }
    else {
        for (int i = 0; i < n; i++) {
            res *= base;
        }
        return res;
    }    
}

unsigned long frame_ceil(unsigned long frame_size, unsigned long size) {
    if (!(size / frame_size)) return 1;
    else if (size % frame_size) return size / frame_size + 1;
    else return size / frame_size; 
}

int WhichPowerOfTwo(unsigned long x) {
    // if note power of 2, return -1
    if (!((x & (x - 1)) == 0)) return -1;
    
    int power = 0;
    while (1) {
        if (x & pow(2, power)) break;
        power++;
    }
    return power;
}

