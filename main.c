#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
enum {black, red};
#define max_tree_size 1024

struct List {
    int page;
    struct node *value;
    struct List *next;
};


struct HashTab {
    int page;
    int size;
    struct List **table;
};

struct Exist {
    short int True;
    int *value;
};

struct buffer {
    struct buffer *next;
    struct node *address;
    long int data;
    long int key;
    long int hist1;
    long int hist2;
    long int last;
};

struct node {
    struct node * parent;
    struct node * left;
    struct node * right;
    int color;
    struct buffer * address; //pointer to element's place in buffer
    int hist1;  //hist1, time of the mos recent uncorrelated reference
    int value; //hist2  , time of second uncorrelated reference
    int last;  // time of the most recent access
};
struct tree {
    struct node * top;
    int size;
};

void free_list(struct List **head);
void print_list(struct List *head);
void set_list(struct List **head, int page, int *value);
void del_in_list(struct List **head, int page);
struct Exist exist_in_list(struct List *head, int page);









struct HashTab *create_hashtab( int size);

void free_hashtab(struct HashTab *hashtab);

int *get_value(struct HashTab *hashtab, int page);

void add_value(struct HashTab *hashtab, int page, int* value);
void del_value(struct HashTab *hashtab, int page);

void print_hashtab(struct HashTab *hashtab);

int hash_funk(int page, int size);


void tree_create(struct tree * tree) {
    tree->size = 0;
    tree->top = 0;
}
void rotate_left(struct tree * tree, struct node *n) {
    // there is no change in color happening
    // n is upper and left element
    // we rotate it with n-> right
    // let n->right be nright
    // our change is : we make n the left son of nright; n->right = nright->left; nright->left = n; n->parent = nright
    struct node * nright;
    nright = n->right;
    // transition of nright->left to n
    n->right = nright->left;
    nright->parent = n->parent;
    if (nright->left)
        nright->left->parent = n;
    if (n->parent) {
        // if n is left son
        if (n->parent->left == n)
            n->parent->left = nright;
        // if n is right son
        else
            n->parent->right = nright;
    }
    else
        tree->top = nright;
    // connect nright and n
    n->parent = nright;
    nright->left = n;
}
void rotate_right(struct tree* tree, struct node *n) {
    // there is no change in color happening
    // n is upper and right element
    // we rotate it with n-> left
    // let n->left be nleft
    // our change is : we make n the right son of nleft; n->left = nleft->right; nleft->right = n; n->parent = nleft
    struct node *nleft = n->left;
    nleft->parent = n->parent;
    if (n->parent) {
        // if n is left son
        if (n->parent->left == n)
            n->parent->left = nleft;
        // if n is right son
        else
            n->parent->right = nleft;
    }
    else
        tree->top = nleft;
        //transition of nleft->right to n
    n->left = nleft->right;
    if (nleft->right)
        nleft->right->parent = n;
    // connect nleft and n
    n->parent = nleft;
    nleft->right = n;
}
// the function returns pointer to a place, where the element with specified value is stored
struct node * tree_insert( struct tree * tree, int value) {
    struct node * x, * parent, * cur, * grandfather;
    if ((tree->size) && (tree->top)) {
        parent = tree->top;
        x = parent;
        for(; ; ) {
            if(parent->value < value)
                x = x->right;
            else
                x = x->left;
            if(x == 0)
                break;
            parent = x;
        }
    }
    else {
        tree->size = 1;
        parent = 0;
    }
    //create new node
    x = (struct node *) malloc(sizeof(struct node));
    assert(x);
    x->color = red;
    x->value = value;
    x->right = 0;
    x->left = 0;
    x->parent = parent;
    if(parent == 0)
        return x;
    if(parent->value < value)
        parent->right = x;
    else
        parent->left = x;
    tree->size++;
    // start balancing
    cur = x;
    for(; ; ) {
        parent = cur->parent;
        // case 1: if cur is root
        if(parent == 0) {
            cur->color = black;
            break;
        }
        //case 2: if parent is black
        if (parent->color == black)
            break;
        grandfather = parent->parent;
        //case 3 : parent is red
        // we want cur be on the same side, as parent is relative to the grandfather, this means,
        // that if parent is the right son, then we should make cur also the right son and vice versa
        if ((cur == parent->right) && (parent == grandfather->left)) {
            rotate_left(tree, parent);
            parent = cur;
            cur = cur->left;
        }
        else if ((cur == parent->left) && (parent == grandfather->right)) {
            rotate_right(tree, parent);
            parent = cur;
            cur = cur->right;
        }
        // cur is on the same side, as parent is relative to the grandfather.
        //now we want to rotate parent and grandfather, so parent will become a new grandfather
        parent->color = black;
        grandfather->color = red;
        if (parent == grandfather->left)
            rotate_right(tree, grandfather);
        else
            rotate_left(tree, grandfather);
        break;
    }
    return x;
}
void tree_delete_fixup(struct tree * tree, struct node * x) {
    struct node * w;
    while((tree->top != x) && (x->color == black)) {
        if(x == x->parent->left) {
            // w is brother of x
            w = x->parent->right;
            // case 1: x has red brother
            // we want this case to be reduced to case 2: x is black and has black brother and red parent
            if(w->color == red) {
                w->color = black;
                x->parent->color = red;
                rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            // case 2: x has black brother and both children of w are black
            // we want to make brother red and parent black
            if((w->left->color == black) && (w->right->color == black)) {
                w->color = red;
                x = x->parent;
            }
            // case 3: brother is black , right child of w is black and left is red
            // we want this case to be reduced to case 4, when w is black and has red right child
            else {
                if (w->right->color == black) {
                    w->left->color = black;
                    w->color = red;
                    rotate_right(tree, w);
                    w = x->parent->right;
                }
                // case 4: w is black and has red right child
                // reduce everything to the beginning of the algorithm but 2 steps higher
                w->color = x->parent->color;
                x->parent->color = black;
                w->right->color = black;
                rotate_left(tree, x->parent);
                break;
            }
        }
        // here we will do the same , but we will change all right to left and vice versa
        else {
            // w is brother of x
            w = x->parent->left;
            // case 1: x has red brother
            // we want this case to be reduced to case 2: x is black and has black brother and red parent
            if(w->color == red) {
                w->color = black;
                x->parent->color = red;
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            // case 2: x has black brother and both children of w are black
            // we want to make brother red and parent black
            if((w->left->color == black) && (w->right->color == black)) {
                w->color = red;
                x = x->parent;
            }
            // case 3: brother is black , right child of w is black and right is red
            // we want this case to be reduced to case 4, when w is black and has red left child
            else {
                if (w->left->color == black) {
                    w->right->color = black;
                    w->color = red;
                    rotate_left(tree, w);
                    w = x->parent->left;
                }
                // case 4: w is black and has red left child
                // reduce everything to the beginning of the algorithm but 2 steps higher
                w->color = x->parent->color;
                x->parent->color = black;
                w->left->color = black;
                rotate_right(tree, x->parent);
                break;
            }
        }
    }
    x->color = black;
}
void tree_delete(struct tree * tree, struct node * z) {
    struct node * x, *y;
    // node z is to be deleted, but instead we will delete node y, but we will copy
    // information from y to z; x is the node that is a child of y;
    // if either of left or right child of z are 0, than y = z, else y = Tree_Succeeder(z),
    // that means, that if z->right != 0, than if is minimum from the right subtree of z
    y = z;
    if((z->left) && (z->right)) {
        // find Tree_Succeeder
        y = y->right;
        while(y->left)
            y = y->left;
    }
    // connecting child of y to parent of y
    if(y->left)
        x = y->left;
    else
        x = y->right;
    x->parent = y->parent;
    if(y->parent == 0)
        tree->top = x;
    else {
        if(y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    // copy information from y to z, then delete y;
    if(y != z) {
        z->address = y->address;
        z->hist1 = y->hist1;
        z->last = y->last;
        z->value = y->value;
    }
    tree->size--;
    // if y is black, then we need to fix the tree, because one black node was removed, and ,thus,
    // the 4-th property of RB tree (equality of quantity of black nodes from one node to any leaf) is violated
    if(y->color == black) {
        free(y);
        tree_delete_fixup(tree,x);
    }
    else
        free(y);
}
// this function should be called only when value is updated
// it returns pointer to the new location of the node in the tree
struct node * tree_update(struct tree* tree, struct node * n, int value) {
    struct node x;
    struct node * y;
    // copy data from the node that is about to be deleted
    x.address = n->address;
    x.hist1 = n->hist1;
    x.last = n->last;
    //deleting old element
    tree_delete(tree, n);
    //inserting it back
    y = tree_insert(tree, value);
    // copying data back
    y->address = x.address;
    y->hist1 = x.hist1;
    y->last = x.last;
    return y;
}
// used by tree_destroy function
// is here just not to give poinnter to tree->top, but rather tree pointer itself
void Destroy_tree_pointer_input(struct node * n) {
    if(n->right)
        Destroy_tree_pointer_input(n->right);
    if(n->left)
        Destroy_tree_pointer_input(n->left);
    free(n);
}
// function that is used to destroy the tree
void tree_destroy(struct tree * tree) {
    Destroy_tree_pointer_input(tree->top);
}

/* Вычисление Хэша для страницы*/
int hash_funk(int page, int size) {
    assert(size > 0);
    if(page < 0)
        page = page * (-1);
    return (page % size);
}

/* Создание хэштаблицы*/
struct HashTab *create_hashtab( int size) {
    assert(size > 0);
    struct HashTab *hashtab = calloc(1, sizeof(struct HashTab));
    struct List *head = NULL;
    hashtab->table = calloc(size, sizeof(struct List));
    for (int i = 0; i < size; ++i) {
        hashtab->table[i] = head;
    }
    hashtab->size = size;
    return hashtab;
}


/* Освобождение памяти*/
void free_hashtab(struct HashTab *hashtab) {
    assert(hashtab != NULL);
    for (int i = 0; i < hashtab->size; ++i) {
        free_list(&(hashtab->table[i]));
    }
    free (hashtab->table);
    free (hashtab);
}


/* Вывод всех элементов которые есть в тыблице*/
void print_hashtab(struct HashTab *hashtab) {
    assert(hashtab != NULL);
    for (int hash = 0; hash < hashtab->size; ++hash) {
        if(hashtab->table[hash] != NULL) {
            printf("В хэше %d находятся:", hash);
            print_list(hashtab->table[hash]);
        }
    }
}


/* Добавление элемента в хэш таблицу*/
void add_value(struct HashTab *hashtab, int page, int *value) {
    assert(hashtab != NULL);
    int hash = hash_funk(page, hashtab->size);
    set_list(&(hashtab->table[hash]), page, value);
}

/* Получение адреса страницы*/
int *get_value(struct HashTab *hashtab, int page) {

    assert(hashtab != NULL);
    struct Exist Need;
    int hash = hash_funk(page, hashtab->size);

    Need = exist_in_list(hashtab->table[hash], page);
    if(Need.True == 0)
        return NULL;
    return Need.value;
}

/* Удаление элемента*/
void del_value(struct HashTab *hashtab, int page) {
    assert(sizeof(page) == 4);
    struct Exist Need;
    int hash = hash_funk(page, hashtab->size);
    Need = exist_in_list(hashtab->table[hash], page);
    if(Need.True != 0)
        del_in_list(&(hashtab->table[hash]), page);
}





/* Добавление элемента в список по определенному жэшу */
void set_list(struct List **head, int page, int *value) {
//    assert(value != NULL);
    struct List *temp = malloc(1 * sizeof(struct List));
    temp->page = page;
    temp->value = value;
    temp->next = *head;
    *head = temp;

}

/* Вывод всех элементов которые есть в списке по определенному хэшу*/
void print_list(struct List *head) {
    if(head != NULL) {
        printf("Страница номер %d и адрес %d \n", head->page, *head->value);
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


/* Проверка существование элеента в таблице и сразу забор value*/
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


/* Удаление элемента из списка*/
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

//--------------------------------------------------------------------


// Create buffer's element

void add_list (struct buffer *buf, long int k, long int t, long int dat, struct node *address) {
    struct buffer *list = (struct buffer*) malloc(sizeof(struct buffer));
    if (k != 1) {
        while (buf->next != NULL) {
            buf = buf->next;
        }
    }
    list->data = dat;
    list->key = k;
    list->hist1 = t;
    list->hist2 = 0;
    list->last = t;
    list->next = NULL;
    list->address = address;
    buf->next = list;
    address->address = list;
    address->hist1 = t;
    address->last = t;
    free(list);
}

// Change last entry

void change_last (struct buffer *buf, long int k, long int nlast) {
    while (buf->key != k) {
        buf = buf->next;
    }
    buf->last = nlast;
    buf->address->last = nlast;
}

// Set second entry

void change_second (struct buffer *buf, long int k, long int sec, struct tree *tree) {
    while (buf->key != k) {
        buf = buf->next;
    }
    if (buf->hist2 == 0) {
        buf->hist2 = sec;
        tree_update(tree, buf->address, sec);
    }
}

// Replace one of buffer's elements

void change_list (struct buffer *buf, long int k, long int t, long int dat, struct tree *tree) {
    struct node *address;
    while (buf->key != k) {
        buf = buf->next;
    }
    buf->data = dat;
    buf->hist1 = t;
    buf->hist2 = 0;
    buf->last = t;
    address = tree_insert(tree, 0);
    address->hist1 = t;
    address->value = 0;
    address->last = t;
    address->address = buf;
    buf->address = address;
}

struct node *calculate_victum (struct buffer *buf) {
    struct node *address_victum = buf->address;
    int time_victum = buf->hist2;
    while (buf->next != NULL) {
        if (buf->hist2 < time_victum) {
            address_victum = buf->address;
            time_victum = buf->hist2;
        }
    }
    return address_victum;
}

void free_buffer(struct buffer *buf) {
    struct buffer *str;
    while (buf->next != NULL) {
        str = buf->next;
        free(buf);
        buf = str;
    }
    free(buf);
}

//

int main() {
    int size, number, i, hits = 0, element, sizehash = 0, hash, victum;
    struct HashTab *hashtab;
    struct Exist check;
    struct buffer *buff;
    struct tree *tree;
    struct node *address, *victum_address;
    tree_create(tree);
    scanf("%d %d", &size, &number);
    hashtab = create_hashtab(sizeof(long int));
    scanf("%d", &element);
    address = tree_insert(tree, 0);
    hash = hash_funk(element, sizeof(long int));
    add_value(hashtab, element, address);
    add_list(buff, hash, 1, element, address);
    sizehash++;
    for (i = 1; i < number; i++) {
        scanf("%d", &element);
	hash = hash_funk(element, sizeof(long int));
        check = exist_in_list(hashtab->table[hash], element);
        if (check.True != 1) {
            if (sizehash != size) {
                address = tree_insert(tree, 0);
                add_value(hashtab, element, address);
                add_list(buff, hash, i, element, address);
                sizehash++;
            }
            else {
                victum_address = calculate_victum(buff);
                victum = victum_address->address->key;
                tree_delete(tree, victum_address);
                del_value(hashtab, victum);
                add_value(hashtab, element, address);
                change_list(buff, victum, i, element, tree);
            }
        }
        else {
               change_second(buff, hash, i, tree);
               change_last(buff, hash, i);
               hits++;
        }
         printf("%d", hits);
    }
    free_hashtab(hashtab);
    free_buffer(buff);
    tree_destroy(tree);
    printf("%d", hits);
}
