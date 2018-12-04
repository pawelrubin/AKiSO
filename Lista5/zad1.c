#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int myprintf(const char * format, ... ) {
  char * start_pointer = format;
  int fd = 0;
  char *buf;
  // va_list args;

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
          // ponizsze konwertowac na char
          // int *d = ((int *)p);
          // write(1, d, sizeof(int)); // to drukuje znak z ASCII o numerze d
          char * d = ((char*)p);
          for (int i = 0; i < strlen(d); i++) {
            int d_int = atoi(d[i]);
            printf("%d", d_int);
          }
          printf("%s", d);
          // int len = strlen(d);
          // int d_int = &d;
          // write(1, d_int-48, len);
          // for (int i = 0; i < len; i++) {
          //   // char d_i = d[i] - '0';
          //   write(1, d+i-', 1);
          // }
          p += sizeof (int);

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

// int mysncanf(const char * format, ... ) {
//   char * start_pointer = format;
//   int fd = 0;
//   char *buf;
//   // va_list args;

//   // va_start(args, format);
//   char *p = (char *) &format + sizeof format;

//   while (*format) {
//     if (*format == '%') {
//       format++;
//       switch(*format) {
//         case 'x': {
          
//           break;
//         }
//         case 'd': {
//           int d = *((int *)p);
//           // printf("%d",d);
//           read(0, &d, 1);
//           p += sizeof (int);

//           break;
//         }
//         case 's': {
//           break;
//         }
//         case 'b': {
//           break;
//         }
//       }
//       format++;
//     } else if (*format == '\\') {
//       format++;
//       switch (*format) {
//         case 'n': {
//           read(0, "\n", 1);
//           break;
//         }
//       }
//       format++;
//     } else {
//       read(0, format, 1);
//       format++;
//     }
//   }

//   p = NULL;

//   return (format - start_pointer);
// }

int main(int argc, char *argv[]) {
  // int d;
  // mysncanf("%d", d);
  int d = 49;
  printf("myprintf(\"Papaj mówi: %%d.\\n\", 0)\n");
  myprintf("Papaj mowi: %d.\n", d);
  // myprintf("\n");
  return 0;
}