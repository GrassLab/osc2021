void dumpArchive();
void loadApp(char* path,unsigned long a_addr,unsigned long a_size);
void loadApp_with_argv(char* path,unsigned long a_addr,char** argv,unsigned long* task_a_addr,unsigned long* task_a_size);