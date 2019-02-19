#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "Debug.h"
#include "symbol.h"

/** @file symbol.c
 *  @brief You will modify this file and implement the symbol.h interface
 *  @details Your implementation of the functions defined in symbol.h.
 *  You may add other functions if you find it helpful. Added functions
 *  should be declared <b>static</b> to indicate they are only used
 *  within this file.
 * <p>
 * @author <b>Your name</b> goes here
 */

/** size of LC3 memory */
#define LC3_MEMORY_SIZE  (1 << 16)

/** Provide prototype for strdup() */
char *strdup(const char *s);

/** Defines the data structure used to store nodes in the hash table */
typedef struct node {
  struct node* next;     /**< linked list of symbols at same index */
  int          hash;     /**< hash value - makes searching faster  */
  symbol_t     symbol;   /**< the data the user is interested in   */
} node_t;

/** Defines the data structure for the symbol table */
struct sym_table {
  int      size;        /**< size of hash table          */
  node_t** hash_table;  /**< array of node_t pointers    */
  char**   addr_table;  /**< array of character pointers */
};

/** djb hash - found at http://www.cse.yorku.ca/~oz/hash.html
 * tolower() call to make case insensitive.
 */

static int symbol_hash (const char* name) {
  unsigned char* str  = (unsigned char*) name;
  unsigned long  hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + tolower(c); /* hash * 33 + c */

  c = hash & 0x7FFFFFFF; /* keep 31 bits - avoid negative values */

  return c;
}

/** @todo Implement this function */
sym_table_t* symbol_init (int table_size) {
  debug("symbol_init was called with table_size = %d", table_size);
  sym_table_t* sym_tab = malloc(sizeof(sym_table_t));
  sym_tab->hash_table = calloc(table_size, sizeof(node_t*));
  sym_tab->addr_table = calloc(LC3_MEMORY_SIZE, sizeof(char*));
  sym_tab->size = table_size;
  return sym_tab;
}

/** @todo Implement this function */
void symbol_add_unique (sym_table_t* symTab, const char* name, int addr) {
  int hash = symbol_hash(name);
  int index = hash % symTab->size;
  //char* newName = strdup(name);
  debug("Hash: %d, index: %d", hash, index);
  node_t* node = calloc(1, sizeof(node_t));
  symbol_t symbol;

  debug("Symbol and node created");

  symbol.name = strdup(name);//newName;
  symbol.addr = addr;
  node->next = NULL;
  node->hash = hash;
  node->symbol = symbol;

  debug("Symbol and node initialized");
  
  if(symTab->hash_table[index] == NULL){
	debug("Index is empty, prepare to add node");
  	symTab->hash_table[index] = node;
	debug("Node added to head of list");
  }
  else{ 
	debug("Index not empty, prepare to insert node");
	node->next = symTab->hash_table[index];
	symTab->hash_table[index] = node;
	debug("Node added to head of list. Connected to next node");
  }
  debug("adding address to addr_table");
  if(symTab->addr_table[addr]==NULL)
  	symTab->addr_table[addr] = symbol.name; 
  debug("address added.\n symTab->addr_table[addr] : %s\n address: %d\n", symTab->addr_table[addr], addr); 

}

/** @todo Implement this function */
char* symbol_find_by_addr (sym_table_t* symTab, int addr) {
  debug("find by address called");
  debug("expected return value: %s\n", symTab->addr_table[addr]);
  return symTab->addr_table[addr];
}

/** @todo Implement this function */
void symbol_iterate (sym_table_t* symTab, iterate_fnc_t fnc, void* data) {
  debug("iterator called successfully");
  for(int i = 0; i < symTab->size; i++){
	node_t* curr = symTab->hash_table[i];
	while(curr!=NULL){
		(*fnc)(&(curr->symbol), data);
		curr = curr->next;
	}
  }
}

/** @todo Implement this function */
struct node* symbol_search (sym_table_t* symTab, const char* name, int* ptrToHash, int* ptrToIndex) {
  debug("Symbol search successfully called");
  *ptrToHash = symbol_hash(name);
  *ptrToIndex = *ptrToHash%(symTab->size);
  debug("Check initialization. *ptrToHash:%d *ptrToIndex:%d name:%s", *ptrToHash, *ptrToIndex, name);

  node_t* curr = symTab->hash_table[*ptrToIndex];
  debug("Iterating linked list at index: %d", *ptrToIndex);
  while(curr!=NULL){
	if(*ptrToHash==curr->hash && strcasecmp(strdup(name), curr->symbol.name)==0){
		debug("symbol found, function terminated\n");
		return curr;
	}
	else	
		curr = curr->next;
	
  }
  debug("symbol NOT currently in table\n");
  return NULL;
}

/** @todo Implement this function */
int symbol_add (sym_table_t* symTab, const char* name, int addr) {
  debug("symbol_add method successfully called");
  int ptrToHash = symbol_hash(name);
  int ptrToIndex = ptrToHash%(symTab->size);

  if(symbol_search(symTab, name, &ptrToHash, &ptrToIndex)==NULL){
	symbol_add_unique(symTab, name, addr);
	debug("Symbol added\n");
	return 1;
  }else{
  	debug("Symbol NOT added\n");
  	return 0;
  }
}

/** @todo Implement this function */
symbol_t* symbol_find_by_name (sym_table_t* symTab, const char* name) {
  debug("symbol_find_by_name successfully called\n");
  int ptrToHash = symbol_hash(name);
  int ptrToIndex = ptrToHash%(symTab->size);
  node_t* symNode = symbol_search(symTab, name, &ptrToHash, &ptrToIndex);

  if(symNode==NULL)
  	return NULL;
  else
	return &(symNode->symbol);
}

/** @todo Implement this function */
void symbol_reset(sym_table_t* symTab) {
  debug("reset successfully called");
  int count = 1;
  
  //loop to empty address table contents
  debug("Setting addresses to NULL");
  for(int i = 0; i < LC3_MEMORY_SIZE; i++){
	symTab->addr_table[i] = NULL;
  }

  //loop to free hash_table symbols
  for(int i = 0; i < symTab->size; i++){
	node_t* curr = symTab->hash_table[i];
	node_t* buff = NULL;
	while(curr!=NULL){
		debug("Freeing memory for node %d", count);
		buff = curr;
		curr = curr->next;
		free(buff->symbol.name);// = NULL;
		debug("Attempting to free node");
		free(buff); buff = NULL;
		debug("Node freed");
		count++;
	}
	symTab->hash_table[i] = NULL;
  }

  debug("reset successfully terminated\n");
}

/** @todo Implement this function */
void symbol_term (sym_table_t* symTab) {
  debug("terminate successfully called");
  symbol_reset(symTab); debug("symbol table reset");
  free(symTab->hash_table); debug("hash_table freed");
  free(symTab->addr_table); debug("address table freed");
  free(symTab);
  debug("symbol table successfully deconstructed. Terminating program\n");
}

