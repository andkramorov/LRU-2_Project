#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "LRU/K_hash.h"

/**
 * A list.
 */
struct List {
    int page;                       /**< a page of data */
    int *value;                     /**< an adress of data. */
    struct List *next;              /**<  a pointer to the next element of the list*/
};

/**
 * A struct which tell us about existence of element in hash table
 */
struct Exist {
    short int True;                 /**< a sign of existence*/
    int *value;                     /**< an adress*/
}

/**
 * Deletion of the list
 * @param head a double pointer to the head of the list
 */
void free_list(struct List **head);

/**
 * This function prints all element in the one list
 * @param head a pointer to the head of the list
 */
void print_list(struct List *head);

/**
 * This function adds a new element in the list
 * @param page a page of data which we add to the list
 * @param value  an adress of data
 */
void set_list(struct List **head, int page, int *value);

/**
 * Deleting the element in the list
 * @param page a page of data which we delete
 * @param head a double pointer to the head of the list
 */
void del_in_list(struct List **head, int page);

/**
 * This function looks for an element in the list
 * @param page a page of data which we look for
 * @param head a pointer to the head of the list
 */
struct Exist exist_in_list(struct List *head, int page);

