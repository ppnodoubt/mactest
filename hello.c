#include <stdio.h>
#include <stdlib.h>

int main(int ac, char **av)
{
	if( ac != 2){
		perror("invalid input");
		exit(1);
	}

	printf("input is : %s\n", av[1]);
	printf("bye\n");
	return 0;
}
