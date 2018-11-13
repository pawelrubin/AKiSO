#include <stdio.h>

int main() {
	for (int c = 0; c < 256; c++) {
		printf("\033[38;5;%dm%d. Hello World!\n", c, c);
	}
	return 0;
}
