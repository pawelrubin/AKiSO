#include <stdio.h>
#include <signal.h>

void all_signal_handler(int i) {
	printf("Signal neutralized! nr %d\n", i);
}

int main() {
	for (int i = 1; i < 65; i++) {
		if (signal(i, all_signal_handler) == SIG_ERR) {
      printf("Did not catch signal nr %d\n", i);
    }
	}
	
	return 0;
}