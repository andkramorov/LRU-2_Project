#include "LRU2_hash.h"
#include "List_for_hash.h"

int hash_funk(int page, int size) {
    assert(size > 0);
    if(page < 0)
        page = page * (-1);
    return (page % size);
}


struct HashTab *create_hashtab( int size) {
    assert(size > 0);
    struct HashTab *hashtab = calloc(1, sizeof(struct HashTab));
    assert(hashtab);
    struct List *head = NULL;
    hashtab->table = calloc(size, sizeof(struct List));
    assert(hashtab->table);
    for (int i = 0; i < size; ++i) {
        hashtab->table[i] = head;
    }
    hashtab->size = size;
    return hashtab;
}



void free_hashtab(struct HashTab *hashtab) {
    assert(hashtab != NULL);
    for (int i = 0; i < hashtab->size; ++i) {
        free_list(&(hashtab->table[i]));
    }
    free (hashtab->table);
    free (hashtab);
}



void print_hashtab(struct HashTab *hashtab) {
    assert(hashtab != NULL);
    for (int hash = 0; hash < hashtab->size; ++hash) {
        if(hashtab->table[hash] != NULL) {
            printf("In hash %d locate:", hash);
            print_list(hashtab->table[hash]);
        }
    }
}



void add_value(struct HashTab *hashtab, int page, struct node *value) {
    assert(hashtab != NULL);
    int hash = hash_funk(page, hashtab->size);
    set_list(&(hashtab->table[hash]), page, value);
}


struct node *get_value(struct HashTab *hashtab, int page) {

    assert(hashtab != NULL);
    struct Exist Need;
    int hash = hash_funk(page, hashtab->size);
    
    Need = exist_in_list(hashtab->table[hash], page);
    if(Need.True == 0)
        return NULL;
    return Need.value;
}


void del_in_list(struct List **head, int page) {
    assert(*head != NULL);
    struct List *top = *head;
    struct List *prev = NULL;
    while((*head)->page != page) {
        prev = *head;
        *head = (*head)->next;
    }
    if((*head)->next == NULL) {
        if(prev != NULL) {
            free(*head);
            prev->next = NULL;
            *head = top;
        } else {
            *head = NULL;
        }
    } else {
        if(prev != NULL) {
            prev->next = (*head)->next;
            free(*head);
            *head = top;
        } else {
            top = top->next;
            free(*head);
            *head = top;
        }
        
    }
}


