#include "../include/vfs.h"
#include "../include/printf.h"
#include "../include/uart.h"
#include "../include/memAlloc.h"
#include "../include/stringUtils.h"


typedef struct _Metadata{
    unsigned char state_of_partition;
    unsigned int partition_beg;//sector # of begin of partition
    unsigned int table_beg;//start of FAT table region
    unsigned int table_num;//number of table
    unsigned int table_size;//number of sectors per FAT
    unsigned int sector_num;//number of sectors in partition
    unsigned int sector_size;//size per sector in bits
    unsigned int data_beg;//begin of data region
    unsigned int *table;//point to begin of FATs
}Metadata;

typedef struct _Dentry{
    unsigned char name1[8];
    unsigned char name2[3];
    unsigned char attr;
    unsigned char other1[8];
    unsigned short id1;
    unsigned char other2[4];
    unsigned short id2;
    unsigned int len;
}Dentry;

typedef struct _Node{
    vnode* parent;
    unsigned char name[13];
    unsigned char type;//1:DIR 2:FILE
    unsigned int id;
    unsigned int len;
    unsigned int dirty;
    unsigned int capacity;
    void* component;
}Node;

Metadata metadata;

void parseDentry(Dentry* dentry, vnode* v_node, vnode* parent, int display){
    unsigned char name[13];
    int cnt =0;
    for(int i =0;i<8;++i){
        if(dentry->name1[i]==' ')break;
        name[cnt++] = dentry->name1[i];
    }
    name[cnt++] = '.';
    for(int i =0;i<3;++i){
        if(dentry->name2[i]==' ')break;
        name[cnt++] = dentry->name2[i];
    }
    name[cnt] = 0;

    unsigned int id = 0;
    id = dentry->id1;
    id = (id<<16) +dentry->id2;
    unsigned int len = dentry->len; // fils size
    unsigned int type = (dentry->attr&0x10)>0;

    if(display){
        uart_printf("Parent: %s, File: %s, Sector:%d,Bytes:%d, directory:%d\n", ((Node*)(parent->internal))->name,name,id,len,type);
    }

    v_node->mount = parent->mount;
    v_node->f_ops = parent->f_ops;
    v_node->v_ops = parent->v_ops;
    v_node->internal = (void*)my_alloc(sizeof(Node));
    Node* new_internal = v_node->internal;
    for(int i =0;i<13;++i){
        new_internal->name[i] = name[i];
    }
    new_internal->component = 0;
    new_internal->parent = parent;
    new_internal->type = type;
    new_internal->id = id;
    new_internal->len = len;
}

unsigned int getChainLen(int id){
    int ret =1;
    while(1){
        id = metadata.table[id];
        if(id>=2 && id<=0xFFFFFEF){
            ret++;
        }else if(id>=0xFFFFFF8 && id<=0xFFFFFFF){
            break;
        }else{
            uart_printf("unknown FAT value\n");
            while(1);
        }

    }
    return ret;
}

unsigned int getChain(unsigned int id, unsigned char** buf){
    unsigned int len = getChainLen(id)*512;
    *buf = (unsigned char*)my_alloc(len);
    for(int i = 0;i<len ; i+=512){
        readblock(metadata.data_beg+id,(*buf)+i);
        id = metadata.table[id];
    }
    return len;
}

void fatLookup(vnode *dir_node, vnode** target, char* component_name){
    Node *node = (Node*)dir_node->internal;
    if(node->type!=1){
        uart_printf("not a directory\n");
        while(1);
    }

    if(node->component == 0){
        unsigned char *buf;
        unsigned int len = getChain(node->id,&buf);
        vnode** childs = (vnode*)my_alloc(len/32*8);
        int cnt =0;
        for(int i = 0 ;i<len;i+=32){
            if(buf[i] == 0)break;
            childs[cnt] = (vnode*)my_alloc(sizeof(vnode));
            parseDentry((Dentry*)(buf+i),childs[cnt],dir_node,1);
            cnt++;
        }
            while(1){}//Continue here;

    }



};

void fatCreate(){
    //TODO;
    while(1);
};
void fatRead(){}
void fatWrite(){};

void parseROOT(vnode* root){
    unsigned char *table = (unsigned char*)my_alloc(metadata.table_size*512);
    uart_printf("table beg:%d, size:%d\n",metadata.table_beg,metadata.table_size);
    for(int i =0;i<metadata.table_size;++i){
        readblock(metadata.table_beg+i,table+i*512);
    }
    if(metadata.table!=0){
        uart_printf("dirty table\n");
        while(1);
    }
    metadata.table = (unsigned int*)table;
    //TODO:findout use of dirty
    root->v_ops = (vnode_operations*)my_alloc(sizeof(vnode_operations));
    root->v_ops->lookup = fatLookup;
    root->v_ops->create = fatCreate;
    root->f_ops = (file_operations*)my_alloc(sizeof(file_operations));
    root->f_ops->write = fatWrite;
    root->f_ops->read = fatRead;
    root->internal = (void*)my_alloc(sizeof(Node));
    Node* node = (Node*)root->internal;
    node->name[0] = '.';
    node->name[1] = 0;
    node->type = 1;
    node->id = 2;//# of cluster
    node->component = 0;
}

void parseFAT32(){
    unsigned char* buf = (unsigned char*)my_alloc(512);
    readblock(metadata.partition_beg,buf);
    uart_printf("sector index of partition:%d\n",metadata.partition_beg);
    if(buf[510]!=0x55 || buf[511]!=0xaa){
        uart_printf("Invalid FAT signature\n");
    }
    //unsigned short sector_size = (unsigned short)buf[11];
    unsigned int sector_size = buf[12];
    sector_size = sector_size<<8 + (unsigned short)buf[11];
    if(sector_size != 512){
        uart_printf("Invalid sector size\n");
    }
     unsigned int cluster_size = (unsigned char)buf[13];
     uart_printf("cluster_size:%d\n",cluster_size);
     unsigned int num_of_reserved_sec = *(unsigned short*)(buf+14);
     uart_printf("reserved sec:%d\n",num_of_reserved_sec);
     unsigned int num_of_FAT = (unsigned char)buf[16];
     uart_printf("num of fat:%d\n",num_of_FAT);
     unsigned int num_of_sec_in_par = buf[20];
     num_of_sec_in_par = num_of_sec_in_par<<8+buf[19];

     if(num_of_sec_in_par !=0){
        uart_printf("Not FAT32\n");
        while(1);
     }

     num_of_sec_in_par = *(unsigned int*)(buf+32);

     unsigned int sec_per_FAT = (unsigned short)buf[22];
     if(sec_per_FAT!=0){
        uart_printf("Not FAT32\n");
        while(1);
    }
     sec_per_FAT = *(unsigned int*)(buf+36);
     uart_printf("sec per fat:%d\n",sec_per_FAT);

     metadata.sector_num = num_of_sec_in_par;
     metadata.sector_size = sector_size;
     metadata.table_beg = metadata.partition_beg + num_of_reserved_sec;
     metadata.table_num = num_of_FAT;
     metadata.table_size = sec_per_FAT;
     metadata.data_beg = metadata.table_beg + num_of_FAT*sec_per_FAT -2 ;//TODO:findout why -2 // root diretory start at cluster #2
    uart_printf("databeg:%d\n",metadata.data_beg);
     my_free(buf);
}

void parseMBR(){
    unsigned char* buf = (unsigned char*)my_alloc(512);
    readblock(0,buf);

    if(buf[510]!=0x55 || buf[511]!=0xaa){
        uart_printf("Invalid MBR Signature\n");
        while(1);
    }

    if(buf[446]!=0x80){
        uart_printf("Inactive partiotion\n");
        while(1);
    }

    char* partition_table = buf+446;
    char partition_type = *(char*)(partition_table+4);
    if(partition_type != 0xb){
        uart_printf("Not FAT32\n");
        while(1);
    }

    for(int i =0;i < 4; ++i){
        buf[i] = partition_table[8+i];
        buf[i+4] = partition_table[12+i];
    }

    unsigned int sec_bet_MAP = *(unsigned int*)buf;
    unsigned int num_in_par = *(unsigned int*)buf+4;
    metadata.partition_beg = sec_bet_MAP;

    my_free(buf);
}
int fatSetup(filesystem* fs, mount *mnt){
    char *name = (char*)my_alloc(6);
    name = "fat32";
    fs->name = name;
    fs->setup_mount = fatSetup;
    mnt->root = (vnode*)my_alloc(sizeof(vnode));
    mnt->fs = fs;
    mnt->root->mount = mnt;
    parseMBR();
    parseFAT32();
    parseROOT(mnt->root);
    mnt->root->v_ops->lookup(mnt->root,0,"KERNEL8.IMG");

    return 0;
}
