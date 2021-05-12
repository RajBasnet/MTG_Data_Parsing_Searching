all: parser search

parser: parser.c
	gcc -std=gnu11 -Wall -Werror -o parser parser.c -g
search: search.c
	gcc -std=gnu11 -Wall -Werror -o search search.c -g
	