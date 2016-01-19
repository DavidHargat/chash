#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct hnode {
	struct hnode *next;
	char         *data;
	size_t         key;
} hnode;

typedef struct htable {
	size_t    size;
	hnode  **nodes;
} htable;

hnode *create_node(){
	hnode *node;

	node = malloc(sizeof(hnode));

	node->key  = 0;
	node->next = NULL;
	node->data = NULL;

	return node;
}

htable *create_table(size_t size){
	htable *table;
	int i;
	
	table = malloc(sizeof(htable));
	
	table->size  = size;
	table->nodes = malloc(sizeof(hnode *) * size);

	// Allocate root nodes.
	for(i=0; i<size; i++)
		table->nodes[i] = create_node();

	return table;
}

void free_node(hnode *node){
	if(node->data != NULL) free(node->data);
	if(node->next != NULL) free_node(node->next);	
	free(node);
}

void free_table(htable *table){
	int i;

	for(i=0; i<table->size; i++)
		free_node(table->nodes[i]);

	free(table->nodes);	
	free(table);
}

size_t hash_index(htable *table, size_t key){
	return (key % table->size);
}

// Generate a key
size_t hash_key(char *src, size_t length){
	size_t i, key, tmp, c;

	i   = 0;
	key = 1;
	tmp = src[0] * (length<<1);

	for(; i<length; i++ ){
		c = src[i];
		
		tmp += c ^ tmp;
		tmp += tmp + (c << 1);
		tmp += (c << 3) * tmp;
		tmp += (c << 11) + (tmp<<3);
		tmp += c;
		
		key += tmp;
	}

	key += key >> 11;
	key ^= tmp << 2;
	key += key << 25;
	key += key >> 1;
	key ^= tmp << 9;
	key += key >> 5;	

	return key;
}

// Find the next available parent in the linked list.
hnode *hash_find(hnode **nodes, size_t key, size_t index){
	hnode  *temp, *parent;

	temp    = nodes[index];
	parent  = temp;

	while(
		temp->next != NULL &&
		temp->key  != key
	){
		parent = temp;
		temp   = temp->next;
	}

	return parent;
}

char hash_set( 
	htable *table, 
	char *key_src,  size_t key_length, 
	char *data_src, size_t data_length ){

	size_t key, index;
	hnode *parent, *node;

	// generate key, generate index, find node.
	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = hash_find(table->nodes, key, index);

	// create new node.
	node = create_node();
	node->key  = key;
	node->data = malloc(data_length);
	memcpy(node->data, data_src, data_length);
	
	// insert root
	if(parent->next == NULL){
		parent->next = node;
		return 1;
	}
	
	// collision / replace
	if(parent->next->key == key){
		free_node(parent->next);
		parent->next = node;
		return 1;
	}
	
	// insert next
	if(parent->next->next == NULL){
		parent->next->next = node;
		return 1;
	}
	
	printf("Set Error: %s, %s\n", key_src, data_src);
	return -1;
}

char *hash_get(htable *table, char *key_src, size_t key_length){
	size_t key, index;
	hnode *parent;

	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = table->nodes[index];

	while(
		parent->next != NULL &&
		parent->key  != key
	){
		parent = parent->next;
	}

	if( parent->key == key )
		return parent->data;
	else
		return NULL;
	
} 

char set(htable *table, char *key, char *data){
	return hash_set(table, key, strlen(key)+1, data, strlen(data)+1);
}

char *get(htable *table, char *key){
	return hash_get(table, key, strlen(key)+1);
}

int main(char argc, char **argv){
	htable *table;

	table = create_table(32);
	
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

	free_table(table);

}


