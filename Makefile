all:
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors symnmf.c -lm -o symnmf

clean:
	rm -f symnmf