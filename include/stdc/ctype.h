#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int tolower(int c);
int isspace(int c);
int toupper(int c);
int isalnum(int c);
int isdigit(int c);
int isxdigit(int c);

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CTYPE_H_INCLUDED
