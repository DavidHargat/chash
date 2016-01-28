all: app

run: app
	./app

app: main.c hash.h
	gcc -o app main.c -I.
