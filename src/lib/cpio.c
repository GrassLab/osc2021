#include "cpio.h"
#include "string.h"
#include "mini_uart.h"

void cpio_read(char *path)
{
    // start from root address
    CPIO_NEWC_HEADER *targetAddr = cpio_find_addr((CPIO_NEWC_HEADER *)RAMFS_ADDR ,path);
    if (targetAddr == 0) {
        puts("could not find the file. \r\n");
    } else {
        char *tmp = (char *)(targetAddr + 1);

        // print target name
        int namesize = cpio_attr_value(targetAddr, C_NAMESIZE) - 1;
        tmp += namesize;
        while (((tmp - (char *)targetAddr) % 4) != 0) {
            *tmp = '\0';
            tmp++;
        }

        for (int i = 0; i < cpio_attr_value(targetAddr, C_FILESIZE); i++, tmp++) {
            putchar(*tmp);
        }

        puts("\r\n");
    }
}

CPIO_NEWC_HEADER * cpio_find_addr(CPIO_NEWC_HEADER *pCurrentFile, char *targetName)
{
    int namesize = cpio_attr_value(pCurrentFile, C_NAMESIZE) - 1;
    
    if (namesize <= 0) {
        /// end of ramfs
        return 0;
    }


    char curName[20];
    memset(curName, 0, sizeof(char) * 20);

    // get current filename
    char *pTemp = (char *)(pCurrentFile + 1);

    // c_namesize is 1 byte greater than real name size
    for (int i = 0; i < namesize; i++, pTemp++) {
        curName[i] = *pTemp;
    }

    if (strcmp(curName, targetName) == 0) {
        // this file is the target
        return pCurrentFile;
    } else {
        char *pNextFile = (char *)(pCurrentFile + 1);
        pNextFile += namesize;
        
        // aligned multiple of four
        while (((pNextFile - (char *)pCurrentFile) % 4) != 0) {
            *pNextFile = '\0';
            pNextFile++;
        }

        pNextFile += cpio_attr_value(pCurrentFile, C_FILESIZE);
        
        while (((pNextFile - (char *)pCurrentFile) % 4) != 0) {
            *pNextFile = '\0';
            pNextFile++;
        }

        return cpio_find_addr((CPIO_NEWC_HEADER *)pNextFile, targetName);
        
    }

    return 0;
}

// convert cpio header attribute from hex string to int
int cpio_attr_value(CPIO_NEWC_HEADER *pRoot, CPIO_ATTR attr)
{
    char vStr[9];
    memset(vStr, 0, 9);

    char *pTemp = (char *)pRoot;

    if (attr == C_MAGIC) {
       for (int i = 0; i < 6; i ++) {
           vStr[i] = pTemp[i];
       } 
    } else {
        // first attr has only 6 byte
        pTemp += (8 * attr - 2);
        for (int i = 0; i < 8; i ++) {
           vStr[i] = pTemp[i];
       } 
    }

    return hextoi(vStr);
}