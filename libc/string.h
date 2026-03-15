#ifndef STRINGS_H
#define STRINGS_H

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
int strlen(char s[]);
void strcopy(char *dst, char *src);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char *s1, char *s2);
/* Split 'src' by spaces into argv[]; returns argc. Max MAX_ARGS tokens. */
#define MAX_ARGS 8
int str_split(char *src, char argv[MAX_ARGS][64]);

#endif
