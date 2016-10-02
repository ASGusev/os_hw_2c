#include <print.h>

#define CHAR 'c'
#define SHORT 's'
#define INT 'i'
#define LONG 'l'
#define LONG_LONG 'e'

#define MAX_NUMBER_LEN 22

char* ull_to_string(char *place, unsigned long long number, int base) {
	place[MAX_NUMBER_LEN] = 0;
	char *symbol = "0123456789ABCDEF";
	
	for (int order = MAX_NUMBER_LEN - 1; order >= 0; order--) {
		place[order] = symbol[number % base];
		number /= base;
	}
	
	char *p = place;
	while (*p == '0') {
		p++;
	}
	if (*p == '\0') {
		p--;
	}
	return p;
}

char* print_number(char *text_buffer, va_list source, char var_type, 
		char rep_type) {
	char *ret = 0;
	if (rep_type == 'd') {
		long long number = 0;
		if (var_type == INT || var_type == SHORT || var_type == CHAR) {
			number = va_arg(source, int);
		}
		else if (var_type == LONG) {
			number = va_arg(source, long);
		}
		else if (var_type == LONG_LONG) {
			number = va_arg(source, long long);
		}
		if (number >= 0) {
			ret = ull_to_string(text_buffer, number, 10);
		}
		else {
			unsigned long long unumber = number - 1;
			unumber = ~unumber;
			ret = ull_to_string(text_buffer, unumber, 10);
			ret--;
			*ret = '-';
		}
	}
	else {
		unsigned long long number = 0;
		switch (var_type) {
			case CHAR: {
				number = va_arg(source, int);
				break;
			}
			case SHORT: {
				number = va_arg(source, int);
				break;
			}
			case INT: {
				number = va_arg(source, unsigned int);
				break;
			}
			case LONG: {
				number = va_arg(source, unsigned long);
				break;
			}
			case LONG_LONG: {
				number = va_arg(source, unsigned long long);
				break;
			}
		}
		switch (rep_type) {
			case 'o': {
				ret = ull_to_string(text_buffer, number, 8);
				ret--;
				*ret = '0';
				break;
			}
			case 'u': {
				ret = ull_to_string(text_buffer, number, 10);
				break;
			}
			case 'x': {
				ret = ull_to_string(text_buffer, number, 16);
				ret--;
				*ret = 'x';
				ret--;
				*ret = '0';
				break;
			}
		}
	}
	return ret;
}

void get_style(const char** pos, char *rep_type, char *var_type) {
	*var_type = INT;
	if (**pos == 'l')  {
		(*pos)++;
		if (**pos == 'l') {
			*var_type = LONG_LONG;
			(*pos)++;
		}
		else {
			*var_type = LONG;
		}
	}
	else if (**pos == 'h') {
		(*pos)++;
		if (**pos == 'h') {
			*var_type = CHAR;
			(*pos)++;
		}
		else {
			*var_type = SHORT;
		}
	}
	*rep_type = **pos;
	(*pos)++;
	if (*rep_type == 'i') {
		*rep_type = 'd';
	}
}

void char_to_serial(char *buffer, int pos, int limit, char symbol) {
	send_char(symbol);
	if (pos > limit && buffer == 0) {}
}

void char_to_buffer(char *buffer, int pos, int limit, char symbol) {
	if (pos < limit) {
		buffer[pos] = symbol;
	}
}

int write_string(char *buffer, int pos, int limit, 
		void write_char(char *, int, int, char), char *str) {
	int len = 0;
	char *p = str;
	while ((*p) != 0) {
		write_char(buffer, pos, limit, *p);
		pos++;
		len++;
		p++;
	}
	return len;
}

int printf_template(const char *format, va_list vals, 
		void write_char(char*, int, int, char), char *buffer, 
		int limit) {
	const char *pos = format;
	int symbols_printed = 0;
	while (*pos) {
		if (*pos == '%') {
			char type = 'd';
			char var_type = INT;
			char text[MAX_NUMBER_LEN + 1];
			pos++;
			get_style(&pos, &type, &var_type);
			
			if (type == 's') {
				char *str = va_arg(vals, char*);
				symbols_printed += write_string(buffer, symbols_printed,
						limit, write_char, str);
			}
			else if (type == 'c') {
				write_char(buffer, symbols_printed, limit, 
						va_arg(vals, int));
				symbols_printed++;
			}
			else {
				char *numb = print_number(text, vals, var_type, type);
				symbols_printed += write_string(buffer, symbols_printed, 
						limit, write_char, numb);
			}
		}
		else {
			write_char(buffer, symbols_printed, limit, *pos);
			symbols_printed++;
			pos++;
		}
	}
	va_end(vals);
	
	return symbols_printed;
}

int vprintf(const char *format, va_list args) {
	return printf_template(format, args, char_to_serial, 0, 0);
}

int printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	return vprintf(format, args);
}

int vsnprintf(char *buffer, int bufsz, const char *format, va_list args) {
	int output_length = printf_template(format, args, char_to_buffer, buffer, bufsz);
	if (output_length < bufsz - 1) {
		buffer[output_length] = '\0';
	}
	else {
		buffer[bufsz - 1] = '\0';
	}
	return output_length;
}

int snprintf(char *buffer, int bufsz, const char *format, ...) {
	va_list args;
	va_start(args, format);
	return vsnprintf(buffer, bufsz, format, args);
}
