#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char * int_to_string(int number, int base) {
  int num_of_digits = 0;
  int number_copy;
  int isNegative = 0;  
  
  if (number < 0) {
    isNegative = 1;
    number *= (-1);
  }

  number_copy = number;

  while(number_copy > 0) {
    number_copy /= base;
    num_of_digits++;
  }

  number_copy = number;

  char *result = malloc((num_of_digits + 1 + isNegative) * sizeof(char));

  if (isNegative) {
    result[0] = '-';
  }

  if (base <= 10) {
    for(int i = (num_of_digits - 1 + isNegative); i >= isNegative; i--) {
      result[i] = number_copy%base + '0';
      number_copy /= base;
    }
  } else {
    char digits[] = {'A', 'B', 'C', 'D', 'E', 'F'};
    int digit;
    for(int i = (num_of_digits - 1 + isNegative); i >= isNegative; i--) {
      digit = number_copy%base;
      if (digit < 10) {
        result[i] = digit + '0';
      } else {
        result[i] = digits[digit%10];
      }
      number_copy /= base;
    }
  }

  result[num_of_digits + isNegative] = '\0';

  return result;
}

int myprintf(const char * format, ... ) {
  int len = 0;;

  char *p = (char *) &format + sizeof format;

  while (*format) {
    if (*format == '%') {
      format++;
      switch(*format) {
        case 'x': {
          int *d = ((int *)p);
          len += myprintf("%s", int_to_string(*d, 16));
          p += sizeof(int);
          break;
        }
        case 'd': {
          int *d = ((int *)p);
          len += myprintf("%s", int_to_string(*d, 10));
          p += sizeof(int);
          break;
        }
        case 's': {
          char *s = *((char **)p);

          while(s[0] != '\0') {
            write(1, s, sizeof(char));
            s++;
            len++;
          }

          p += sizeof(char*);
          break;
        }
        case 'b': {
          int *d = ((int *)p);
          len += myprintf("%s", int_to_string(*d, 2));
          p += sizeof(int);
          break;
        }
      }
      format++;
    } else if (*format == '\\') {
      format++;
      switch (*format) {
        case 'n': {
          write(1, "\n", 1);
          len++;
          break;
        }
      }
      format++;
    } else {
      write(1, format, 1);
      len++;
      format++;
    }
  }

  p = NULL;

  return len;
}

int myscanf(const char * format, ...) {
  int len = 0;;

  char *p = (char *) &format + sizeof format;

  while (*format) {
    if (*format == '%') {
      format++;
      switch(*format) {
        case 'x': {
          int *d = ((int *)p);
          len += myprintf("%s", int_to_string(*d, 16));
          p += sizeof(int);
          break;
        }
        case 'd': {
          // char d = *((char *)p);
          char *test;
          read(0, &test, 512);
          myprintf("%d\n", test);
          p += sizeof(int);
          break;
        }
        case 's': {
          char *s = *((char **)p);

          while(s[0] != '\n') {
            read(1, s, sizeof(char));
            s++;
            len++;
          }

          p += sizeof(char*);
          break;
        }
        case 'b': {
          int *d = ((int *)p);
          len += myprintf("%s", int_to_string(*d, 2));
          p += sizeof(int);
          break;
        }
      }
      format++;
    } else if (*format == '\\') {
      format++;
      switch (*format) {
        case 'n': {
          write(1, "\n", 1);
          len++;
          break;
        }
      }
      format++;
    } else {
      write(1, format, 1);
      len++;
      format++;
    }
  }

  p = NULL;

  return len;
}


int main(int argc, char *argv[]) {
  int len = myprintf("%s %d %b %x %s\n", "hello world", -1234, -1234, -1234, "hello world");
  myprintf("dlugosc powyzszego: %d (znak nowej linii uwzgledniony)\n", len);
  myprintf("%s", int_to_string(-2137, 10));
  int *test;
  int fd[2];
  return 0;
}