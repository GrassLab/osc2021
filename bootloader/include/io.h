#pragma once

unsigned long long int hex2int(char *hex);
int is_digit(char ch);
char read_b();
char read_c();
char *read_s(char *str, int max_size);
int read_i();
unsigned long long int read_h();
void print_c(char ch);
void print_s(char *ch);
void print_i(int x);
void print_h(int x);
