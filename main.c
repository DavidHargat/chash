#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 
* @author David Hargat
* @description
*
* A hash table is an associate array.
* That is to say, instead of associating "indices"
* with values you associate "keys".
* 
* In this implementation, "keys" are arbitrary
* length byte arrays, and "values" are likewise.
*
* When you update (set) the hash table, you actually
* copy the value into some index of an array, or
* to be more precise, into a list of list's.
*
* The key determines which list the data is copied into.
*
* Wrappers are defined for easy storage/retrieval of strings.
* 	set( table, key, data) : for associating 2 strings.
* 	get( table, key )      : for retrieval of a string.
*/

typedef struct hnode {
	struct hnode *next;
	char         *data;
	size_t         key;
} hnode;

typedef struct htable {
	size_t    size;
	hnode  **nodes;
} htable;

/*
* create_node
*
* Allocate memory for a new node (linked list).
*
* returns: pointer to a new node
*/
hnode *create_node(){
	hnode *node;

	node = malloc(sizeof(hnode));

	node->key  = 0;
	node->next = NULL;
	node->data = NULL;

	return node;
}

/*
* create_table
*
* Allocate memory for a new hash table.
*
* size: the size of the hash table (how many indices it has)
*       this should reflect roughly how many keys you want to store
*
* returns: pointer to a new hash table
*/
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

/*
* free_node
*
* Properly free()'s an hnode by recursively freeing 
* it's child nodes and associated data.
*
* node: the node to free
*/
void free_node(hnode *node){
	if(node->data != NULL) free(node->data);
	if(node->next != NULL) free_node(node->next);	
	free(node);
}

/*
* free_table
*
* Properly free()'s a htable with all of
* it's contents.
*
* table: the table to free
*/
void free_table(htable *table){
	int i;

	for(i=0; i<table->size; i++)
		free_node(table->nodes[i]);

	free(table->nodes);	
	free(table);
}

/*
* hash_index
* 
* table:   hash table to calculate an index for
* key:     a hashed key
*
* returns: an index of the `table`
*/
size_t hash_index(htable *table, size_t key){
	return (key % table->size);
}

/*
* hash_key
*
* src:    an unhashed key (an array of bytes)
* length: length of key
*
* returns: a hashed key (an unsigned integer)
*/
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

/*
* hash_find
* 
* nodes: an array of linked lists
* key:   the key you're looking for
* index: the index of the linked list to search
*
* returns: 
* if key is found:  parent node           (where next->key == key)
* key is not found: last node in the list (where next is NULL)
*/
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

/* 
* hash_set
*
* table:       table you want to update
* key_src:     pointer to your key
* key_length:  length of your key
* data_src:    pointer to the data you want to store
* data_length: length of the data you want to store
*
* return:      success:  1
*              failure: -1
*/
char hash_set( 
	htable *table, 
	char *key_src,  size_t key_length, 
	char *data_src, size_t data_length ){

	size_t key, index;
	hnode *parent, *node;

	// generate key, calculate index, find available spot in list
	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = hash_find(table->nodes, key, index);

	// create new node.
	node       = create_node();
	node->key  = key;
	node->data = malloc(data_length);
	
	// copy data into node
	memcpy(node->data, data_src, data_length);
	
	// insert at root
	if( parent->next == NULL ){
		parent->next = node;
		return 1;
	}
	
	// collision / replace
	if( parent->next->key == key ){
		free_node(parent->next);
		parent->next = node;
		return 1;
	}
	
	// insert next (this will be the case for non-root insertions)
	if( parent->next->next == NULL ){
		parent->next->next = node;
		return 1;
	}
	
	// we should never get here
	return -1;
}

/*
* hash_get
*
* table:      table you want to search
* key_src:    pointer to your key
* key_length: length of your key
*
* returns:    success: the data (pointer) associated with the key 
*             failure: NULL (key was not found)
*/
char *hash_get(htable *table, char *key_src, size_t key_length){
	size_t key, index;
	hnode *parent;

	key    = hash_key(key_src, key_length);
	index  = hash_index(table, key);
	parent = table->nodes[index];

	// Find the next 'available' spot in the linked list
	// Check for NULL to avoid seg faults.
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

void test_strings(){
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

int main(char argc, char **argv){
	test_strings();
}


