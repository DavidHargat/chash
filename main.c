#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Node linked list structure.
typedef struct hash_node{
	void *data;
	void *next;
} hnode_t;

// Table structure.
typedef struct hash_table{
	unsigned int size;
	hnode_t **nodes;
} htable_t;

// Allocate memory for node (table entry).
hnode_t *create_node(){
	hnode_t *nod;
	nod = malloc(sizeof(hnode_t));
	nod->data = NULL;
	nod->next = NULL;
	return nod;
}

// Allocate memory for table.
htable_t *create_table(unsigned int size){
	htable_t *tab;
	int i;
	
	tab = malloc(sizeof(htable_t));
	tab->size = size;
	tab->nodes = malloc(sizeof(hnode_t *)*size);
	
	// Populate table with root nodes.
	for(i=0; i<size; i++){
		tab->nodes[i] = create_node();
	}
	
	return tab;
}

void free_node(hnode_t *node){
	if(node->data!=NULL) free(node->data);
	if(node->next!=NULL) free_node(node->next);
	free(node);
}

void free_table(htable_t *table){
	int i;
		
	// Free every node.
	for(i=0; i<table->size; i++){
		free_node(table->nodes[i]);
	}
	free(table->nodes);
	free(table);
}

// Generate key from arbitrary data.
//   return `key` on success.
//   return -1 on failure.
int hash_key(htable_t *table, void *src, size_t size){
	unsigned int key, i;
	
	key = 0;
	for(i=0; i<size; i++){
		key += ((char *)src)[i];
	}

	key = (key % table->size);

	return key;
}

// Hash arbitrary data.l
//   return  1 on success.
//   return -1 on failure.
char hash_store(htable_t *tab, void *key_src, size_t key_size, void *data_src, size_t data_size){
	int key, i;
	hnode_t *nod, *find;
	
	// Generate key.
	key = hash_key(tab, key_src, key_size);

	// Create a hash node.
	nod = create_node();
	nod->data = malloc(data_size);

	// copy the data to the node
	memcpy(nod->data, data_src, data_size);

	// Free the old node.
	free_node(tab->nodes[key]);
	
	// search 'bucket' table[key] for empty slot
	tab->nodes[key] = nod;
	
	/*
	i=0;
	while(find->next != NULL){
		find = find->next;
		i++;
	}
	find->next = nod;
	*/

	// success
	return 1;
}

void *hash_get(htable_t *tab, void *key_src, size_t key_size){
	int key = hash_key(tab, key_src, key_size);
	// TEMP: return root node.
	return tab->nodes[key]->data;
}

//  Wrapper for hashing strings.
char set(htable_t *tab, char *key, char *data){
	return hash_store(tab, key, strlen(key)+1, data, strlen(data)+1);
}

// Wrapper for getting string keys.
char *get(htable_t *tab, char *key){
	return (char *) hash_get(tab, key, strlen(key)+1);
}

int main(char argc, char *argv[]){
	htable_t *tab;
	char *result;

	// Create a hash_table
	tab = create_table(32);	

	// Copy strings into the table
	set(tab, "a", "cat");    // tab["a"] = "cat"
	set(tab, "b", "dog");    // tab["b"] = "dog"
	set(tab, "c", "mouse");  // tab["c"] = "mouse"

	// Get them back out with associated keys.
	printf("result = %s\n", get(tab, "a")); // tab["a"] returns "cat"
	printf("result = %s\n", get(tab, "b")); // tab["b"] returns "dog"
	printf("result = %s\n", get(tab, "c")); // tab["c"] returns "mouse"

	// Free memory
	free_table(tab);
	
	return 0;
}

