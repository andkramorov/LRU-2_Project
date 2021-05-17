#include "List_for_hash.h"

void set_list(struct List **head, int page, struct node *value) {
    assert(value != NULL);
    struct List *temp = malloc(1 * sizeof(struct List));
    assert(temp);
    temp->page = page;
    temp->value = value;
    temp->next = *head;
    *head = temp;
    
}

void print_list(struct List *head) {
    if(head != NULL) {
        printf("Page number is %d and it's adress %d \n", head->page, *head->value);
    if(head->next != NULL)
        print_list(head->next);
    }
}


void free_list(struct List **head) {
    if (*head != NULL) {
        while((*head)->next != NULL)
            *head = (*head)->next;
        free(*head);
    } else {
        free(*head);
    }
}


struct Exist exist_in_list(struct List *head, int page) {
    struct Exist exist;
    struct List *temp = head;
    exist.True = 0;
    while(temp != NULL) {
        if(temp->page == page) {
            exist.True = 1;
            exist.value = temp->value;
            break;
        }
        temp = temp->next;
    }
    return exist;
}


void del_in_list(struct List **head, int page) {
    struct List *top = *head;
    struct List *prev = NULL;
    while((*head)->page != page) {
        prev = *head;
        *head = (*head)->next;
    }
    if((*head)->next == NULL) {
        free(*head);
        if(prev != NULL)
            *head = top;
    } else {
        prev->next = (*head)->next;
        free(*head);
        *head = top;
    }
}


