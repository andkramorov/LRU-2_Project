#ifndef RB_TREE_H_INCLUDED
#define RB_TREE_H_INCLUDED
#pragma once
// to create an RB tree, one must create an object of struct tree;
// than tree_create function should be called (this function acts as a constructor)
// tree balancing is based on value space in the node, that alse acts as hist2
// to add element to the tree one must call function tree_create
// to change value field in the node tree_update function should be called
// but if one wants to change any other field, one must not call the tree_updated function, but rather
// just access the node using the pointer
// to delete an element one must call tree_delete function
// to free the memory, that the tree consumes, one should call tree_destroy function
//
struct node {
    struct node * parent;
    struct node * left;
    struct node * right;
    int color;
    void * address; //pointer to element's place in buffer
    int hist1;  //hist1, time of the mos recent uncorrelated reference
    int value; //hist2  , time of second uncorrelated reference
    int last;  // time of the most recent access
};
struct tree {
    struct node * top;
    int size;
};
// should be called upon reation of the tree, acts as a constructor
void tree_create(struct tree * tree);
// the function returns pointer to a place, where the element with specified value is stored
struct node * tree_insert( struct tree * tree, int value);
// the function is used to delete the element from the tree
void tree_delete(struct tree * tree, struct node * z);
// the function is used to update value field in the node, but should not be used for anythyng else
struct node * tree_update(struct tree* tree, struct node * n, int value)
// function to free the memory and destroy the tree
void tree_destroy(struct tree * tree);
// function , that is used to find node with certain value, return pointer to that node
struct node * tree_find(struct tree * tree, int value);
// returns the element with the smallest value(that is located to the left of the tree)
struct node * min_value_node(struct tree* tree);
// function for testing
void tree_test();


#endif // RB_TREE_H_INCLUDED
