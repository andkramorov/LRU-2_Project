#include "LRU2_hash.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct node {
    struct node * parent;
    struct node * left;
    struct node * right;
    int color;
    struct buffer * address;
    int hist1; 
    int value; 
    int last;  
};


void rand_test();

int main() {
    srand(time(NULL));
    
    rand_test();
    return 0;
}

void rand_test() {
    int size = abs(rand()%1000 + 1);
    int sizearr = (abs(rand()%100 + 1)) * size;
    int *arr = calloc(sizearr, sizeof(int));
    assert(arr);
    struct HashTab *hashtab = create_hashtab(size);
    struct node *value = (struct node*) calloc (1, sizeof (struct node));
    assert(value);
    printf("Size of hashtab: %d Pages: %d\n",size, sizearr);
    for (int i = 0; i < sizearr; ++i) {
         add_value(hashtab, arr[i], value);
    }
    for (int i = 0; i < sizearr; ++i) {
        assert(get_value(hashtab, arr[i]) != NULL);
    }
    for (int i = 0; i < sizearr; ++i) {
        del_value(hashtab, arr[i]);
    }
    for (int i = 0; i < sizearr; ++i) {
        assert(get_value(hashtab, arr[i]) == NULL);
    }
    free_hashtab(hashtab);
    printf("Test complete");
}
