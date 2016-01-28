#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

void test_strings(){
	htable *table;

	table = create_table(32);

	// Wrappers
	char  (* set)(htable*, char*, char*);
	char* (* get)(htable*, char*);
	set = &hash_sets;
	get = &hash_gets;

	set(table, "a", "aaa");
	set(table, "b", "bbb");
	set(table, "c", "ccc");
	set(table, "d", "ddd");
	set(table, "e", "e");
	set(table, "f", "f");
	set(table, "g", "g");
	set(table, "h", "h");
	set(table, "i", "i");
	set(table, "j", "j");
	set(table, "k", "k");
	set(table, "l", "l");
	set(table, "i", "XXX");
	
	printf("[%s] = %s\n", "a", get(table, "a"));
	printf("[%s] = %s\n", "b", get(table, "b"));
	printf("[%s] = %s\n", "c", get(table, "c"));
	printf("[%s] = %s\n", "d", get(table, "d"));
	printf("[%s] = %s\n", "e", get(table, "e"));
	printf("[%s] = %s\n", "f", get(table, "f"));
	printf("[%s] = %s\n", "g", get(table, "g"));
	printf("[%s] = %s\n", "h", get(table, "h"));
	printf("[%s] = %s\n", "i", get(table, "i"));
	printf("[%s] = %s\n", "j", get(table, "j"));
	printf("[%s] = %s\n", "k", get(table, "k"));
	printf("[%s] = %s\n", "l", get(table, "l"));
	printf("[%s] = %s\n", "None?", get(table, "none"));

	free_table(table);
}

int main(char argc, char **argv){
	test_strings();
}

