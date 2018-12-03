#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

int myprintf(const char * format, ... ) {
  char * start_pointer = format;
  int fd = 0;
  char *buf;
  va_list args;

  // va_start(args, format);
  char *p = (char *) &format + sizeof format;

  while (*format) {
    if (*format == '%') {
      format++;
      switch(*format) {
        case 'x': {
          
          break;
        }
        case 'd': {
          int d = *((int *)p);
          write(1, *((int *)p), 4);
          printf("mordo tu bylo d");

          break;
        }
        case 's': {
          break;
        }
        case 'b': {
          break;
        }
      }
      format++;
    } else if (*format == '\\') {
      format++;
      switch (*format) {
        case 'n': {
          write(1, "\n", 1);
          break;
        }
      }
      format++;
    } else {
      write(1, format, 1);
      format++;
    }
  }

  p = NULL;

  return (format - start_pointer);
}

int main(int argc, char *argv[]) {
  myprintf("Papaj mowi: %d\n", 2137);
  myprintf("\n");
  return 0;
}