all: app

run: app
	./app

app: main.c
	gcc -o app main.c -I.
