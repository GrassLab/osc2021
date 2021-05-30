void dupPT(void* page_table_src,void* page_table_dst,int level);
void removePT(void* page_table,int level);
void* updatePT(void* page_table0,void* va);
void initPT(void** page_table);