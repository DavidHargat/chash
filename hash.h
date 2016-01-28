#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// @author David Hargat

// linked list //
typedef struct hnode {
	struct hnode *next;
	char         *data;
	size_t         key;
} hnode;

typedef struct htable {
	size_t    size;
	hnode  **nodes;
} htable;

// Headers //
hnode   *create_node();
htable  *create_table(size_t size);

void     free_node(hnode *node);
void     free_table(htable *table);

size_t   hash_index(htable *table, size_t key);
hnode   *hash_find(hnode *root, size_t key);

char hash_set( 
	htable *table, 
	char *key_src,  size_t key_length, 
	char *data_src, size_t data_length);

char *hash_get(htable *table, char *key_src, size_t key_length);

char  set(htable *table, char *key, char *data);
char *get(htable *table, char *key);

// Memory Management //
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

// Hashing //
size_t hash_index(htable *table, size_t key){
	return (key % table->size);
}

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

// Find a node which either matches the key, or has a NULL child
hnode *hash_find(hnode *root, size_t key){
	hnode  *temp, *parent;

	temp   = root;
	parent = root;

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

	// generate key, calculate index, find available spot in list
	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = hash_find(table->nodes[index], key);

	// create new node.
	node       = create_node();
	node->key  = key;
	node->data = malloc(data_length);
	
	// copy data into node
	memcpy(node->data, data_src, data_length);
	
	if( parent->next == NULL ){
		// insert at root
		parent->next = node;
	}else{
		if( parent->next->key == key ){
			// collision / replace
			free_node(parent->next);
			parent->next = node;
		}else{
			// insert
			parent->next->next = node;
		}
	}
}

char *hash_get(htable *table, char *key_src, size_t key_length){
	size_t key, index;
	hnode *parent;

	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = hash_find(table->nodes[index], key);	

	// Key was not found
	if( parent->next == NULL ) return NULL;

	return parent->next->data;	
} 

// string wrapper for hash_set
char hash_sets(htable *table, char *key, char *data){
	return hash_set(table, key, strlen(key)+1, data, strlen(data)+1);
}

// string wrapper for hash_get
char *hash_gets(htable *table, char *key){
	return hash_get(table, key, strlen(key)+1);
}

