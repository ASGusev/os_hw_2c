#ifndef __PRINT_H__
#define __PRINT_H__

#include <serial.h>
#include <stdarg.h>

int printf(const char *format, ...);

int vprintf(const char *format, va_list vals);

int snprintf(char *buffer, int bufsz, const char *format, ...);

int vsnprinft(char *buffer, int bufsz, const char *format, 
	va_list vals);

#endif /*__PRINT_H__*/
