#include <stdio.h>
#include <unistd.h>

int myprintf(const char * format, ... ) {
  char * start_pointer = format;
  int fd = 0;
  char *buf;

  while (*format) {
    if (*format == '%') {
      format++;
      switch(*format) {
        case 'x': {
          break;
        }
        case 'd': {
          break;
        }
        case 's': {
          break;
        }
        case 'b': {
          break;
        }
      }
    } else {
      format++;
    }
  }

  return (format - start_pointer);
}

int main() {

  int i = myprintf("12345678", "kolego", 4);
  printf("%d\n", i);
  return 0;
}