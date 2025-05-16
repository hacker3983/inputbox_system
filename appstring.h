#ifndef _APPSTRING_H
#define _APPSTRING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#endif

char* app_string_stringtolower(char** string, size_t wlen);
wchar_t* app_string_widetolower(wchar_t** wstring, size_t len);
wchar_t* app_string_stringtowide(const char* string);
char* app_string_widetoutf8(wchar_t* wstring);
char* app_string_strcasestr(char* haystack, char* needle);
char* app_string_dupstr(const char* string, size_t len);
char* app_string_getutf8_char(const char* utf8_string, size_t* index, size_t utf8_stringlen);
bool app_string_concatstr(char** destination_string, const char* source_string);
bool app_string_isascii(int c);
#endif