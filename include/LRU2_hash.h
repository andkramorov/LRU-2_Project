#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "List_for_hash.h"

/**
 * A hash table
 */
struct HashTab {
    int size;                       /**< a size of  hash table*/
    struct List **table;            /**< an array of pointers to lists, in which the elements are*/
};

/**
 * A struct which tell us about existence of element in hash table
 */
struct Exist {
    short int True;                 /**< a sign of existence*/
    struct node *value;                     /**< an adress*/
};


/**
 * Create a hash table
 * @param size a size of future hash table
 * @return a pointer to the new hash table
 */
struct HashTab *create_hashtab( int size);


/**
 * Deleating all hash table
 * @param hashtab a pointer to the hash table
 */
void free_hashtab(struct HashTab *hashtab);


/**
 * Deleating all hash table
 * @param hashtab a pointer to the hash table
 * @param page a page of data
 * @return an adress
 */
struct node *get_value(struct HashTab *hashtab, int page);


/**
 * Adding the page in the hash table
 * @param hashtab a pointer to the hash table
 * @param page a page of data
 * @param value an adress
 */
void add_value(struct HashTab *hashtab, int page, struct node * value);

/**
 * Deleting the page in the hash table
 * @param hashtab a pointer to the hash table
 * @param page a page of data
 */
void del_value(struct HashTab *hashtab, int page);

/**
 * Printing all elements, which are in hash table
 * @param hashtab a pointer to the hash table
 */
void print_hashtab(struct HashTab *hashtab);

/**
 * Calculate a hash of the page
 * @param size the size of the hash table
 * @param page a page of data
 * @return the hash of the page
 */
int hash_funk(int page, int size);
