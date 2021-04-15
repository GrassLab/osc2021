#ifndef STRING_H
#define STRING_H

int   strcmp     ( char * s1, char * s2 );
void  strset     ( char * s1, int c, int size );
int   isdigit    ( const char c);
int   strlen     ( char * s );
int   atoi       ( const char * c);
void  itoa       ( int x, char str[], int d);
void  itohex_str ( long long int d, int size, char * s );
void  ftoa       ( float n, char* res, int afterpoint ); 
void  reverse    ( char *s );

#endif
