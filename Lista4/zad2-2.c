#include <stdio.h>
#include <signal.h>

int main() {
	printf("DISCLAILMER: Przy odpaleniu z uprawnieniami roota sygnały zostaną przyjęte przez inita. Nastąpi restart systemu.\n");
	printf("Próba zabicia INITA:  \n");
	sleep(1);
	for (int i = 65; i > 0; i--) {
		printf("\nSygnał nr %d odpali się za 1 sekundę.\n", i);
		sleep(1);
		printf("Proba zabicia inita. Kod wyjsciowy funkcji kill: %d\n", kill(1, i));
	}
	printf("\033[33;01mStayin' Alive!\n");
	return 0;
}
