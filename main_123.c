#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#define data_size 64
#define Correleted_Reference_period 100
enum {black, red};

struct node {
    struct node * parent;
    struct node * left;
    struct node * right;
    int color;
    void * address; //pointer to element's place in buffer
    int hist1;  //hist1, time of the mos recent uncorrelated reference
    int value; //hist2  , time of second uncorrelated reference
    int last;  // time of the most recent access
    int key;
};
struct tree {
    struct node * top;
    int size;
};

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
    struct node *value;
};
void free_list(struct List **head);
void print_list(struct List *head);
void set_list(struct List **head, int page, struct node *value);
void del_in_list(struct List **head, int page);
struct Exist exist_in_list(struct List *head, int page);
struct HashTab *create_hashtab( int size);

void free_hashtab(struct HashTab *hashtab);

struct node *get_value(struct HashTab *hashtab, int page);

void add_value(struct HashTab *hashtab, int page, struct node* value);
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
    // assert(x);
    x->color = red;
    x->value = value;
    x->right = 0;
    x->left = 0;
    x->parent = parent;
    if(parent == 0) {
        x->color = black;
        tree->top = x;
        return x;
    }
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
    if((z->left == 0) && (z->right == 0)) {
        if(z->parent == 0) {
            tree->top = 0;
            tree->size = 0;
            free(z);
            return;
        }
        else {
            tree->size--;
            if(z->parent->left == z)
                z->parent->left = 0;
            else
                z->parent->right = 0;
        }
        if(z->color == black)
            tree_delete_fixup(tree, z->parent);
        free(z);
        return;
    }
    y = z;
    if((z->left) && (z->right)) {
        // find Tree_Succeeder
        y = y->right;
        while(y->left)
            y = y->left;
        x = y->right;
        if(y->parent->left == y)
            y->parent->left = x;
        else
            y->parent->right = x;
        if(x)
            x->parent = y->parent;
    }
    // connecting child of y to parent of y
    else {
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
    if(n == 0)
        return;
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
//internal function of tree_print
void Print_tree(struct node * n) {
    if(n == 0)
        return;
    printf("\n %d r ", n->value);
    if(n->right)
        printf("%d ", n->right->value);
    else
        printf("-1");
    printf(" l ");
    if(n->left)
        printf("%d ", n->left->value);
    else
        printf("-1");
    printf(" ");
    if(n->right)
        Print_tree(n->right);
    if(n->left)
        Print_tree(n->left);
}
// function for printing elements of the tree
void tree_print(struct tree * tree) {
    printf("\n Printing tree \n size %d ", tree->size);
    Print_tree(tree->top);
}
// function for finding node with specyfied value in the tree
struct node * tree_find(struct tree * tree, int value) {
    struct node * x, * parent;
    if(tree->top == 0)
        return 0;
    if ((tree->size) && (tree->top)) {
        parent = tree->top;
        x = parent;
        for(; ; ) {
            if(parent->value == value)
                return  parent;
            if(parent->value < value)
                x = x->right;
            else
                x = x->left;
            if(x == 0)
                break;
            parent = x;
        }
    }
    return 0;
}
// returns the element with the smallest value(that is located to the left of the tree)
struct node * min_value_node(struct tree* tree) {
    struct node * cur;
    cur = tree->top;
    while(cur->left)
        cur = cur->left;
    return cur;
};
//function for testing
void tree_test() {
    struct tree tree;
    struct tree * t;
    struct node * t1;
    int i, n, k;
    int a[1000];
    t = &tree;
    tree_create(t);
    tree_insert(t, 1);
    tree_insert(t, 798);
    //tree_insert(t, 2);
    tree_delete(t, tree_find(t, 1));

    n = 1000000;
    for(i = 0; i < n; i++) {
        tree_insert(t, i);
    }
    for(i = n-1; i>=0 ;i--) {
        tree_delete(t, tree_find(t, i));
    }
    /*
    for(i = 0; i < 1000; i++) {
        tree_insert(t, i);
        a[i] = 1;
    }
    k = 0;
    while(k < 999) {
        i = rand()%1000;
        if(a[i] == 1) {
            a[i] = 0;
            if(tree_find(t, i) == 0)
                printf(" Error, %d not found ", i);
            tree_delete(t, tree_find(t, i));
            k++;
        }
    }
    */
    for(i = 0; i < 10000; i++) {
        tree_insert(t, i);
    }
    for(i = 0; i <10000; i++) {
        tree_update(t, tree_find(t, i), i+50000);
    }
    for(i = 0; i <10000; i++) {
        tree_delete(t, tree_find(t, i+50000));
    }
    printf("%d", t->top->value);
    //tree_print(t);
    tree_destroy(t);
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
void add_value(struct HashTab *hashtab, int page, struct node *value) {
    assert(hashtab != NULL);
    int hash = hash_funk(page, hashtab->size);
    set_list(&(hashtab->table[hash]), page, value);
}

/* Получение адреса страницы*/
struct node *get_value(struct HashTab *hashtab, int page) {

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
void set_list(struct List **head, int page, struct node *value) {
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
/*
void add_list (struct buffer *buf, long int key) {
    struct buffer *list = (struct buffer*) malloc(sizeof(struct buffer));
    if (key != 1) {
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
*/
/*struct node *calculate_victum (struct tree * tree) {
    struct node * cur;
    cur = min_value_node(tree);
    while (buf->next != NULL) {
        if (buf->hist2 < time_victum) {
            address_victum = buf->address;
            time_victum = buf->hist2;
        }
    }
    return address_victum;
}
/*
void free_buffer(struct buffer *buf) {
    struct buffer *str;
    while (buf->next != NULL) {
        str = buf->next;
        free(buf);
        buf = str;
    }
    free(buf);
}
*/
//

int main() {
    // size is the size of the buffer
    // numeber - number of elements on the input
    // counter is the number of elements in the table
    int size, number, i, hits = 0, element, counter = 0, hash, victim, max_tree_size, buffer_counter, correl_period_of_refd_page;
    struct HashTab *hashtab;
    void * data;
    // to check if element exists in the table
    struct Exist check;
    struct tree tree;
    struct tree * t;
    struct node *address, *victim_address;
    t = &tree;
    tree_create(t);
    scanf("%d %d", &size, &number);
    assert(size > 0);
    assert(number > 0);
    hashtab = create_hashtab(size);
    buffer_counter = 0;
    max_tree_size = 10*size;
    for (i = 0; i < number; i++) {
        scanf("%d", &element);
        assert(element >= 0);
        hash = hash_funk(element, size);
        check = exist_in_list(hashtab->table[hash], element);
        if (check.True != 1) {
            // does not exist in the table
            if (counter != max_tree_size) {
                address = tree_insert(t, 0);
                address->address = 0;
                address->hist1 = i;
                address->key = element;
                address->last = i;
                add_value(hashtab, element, address);
                // need to add element to the buffer
                counter++;
                check = exist_in_list(hashtab->table[hash], element);
            }
            else {
                // to do smth later
            }
        }

        if(check.value->address == 0) {
            if(buffer_counter == size) {
                // to be continued
                victim_address = min_value_node(t);
                check.value->address = victim_address->address;
                victim_address->address = 0;
                //*check.value->address = data
                tree_update(t, check.value, check.value->value);
                check.value->hist1 = i;
                check.value->last = i;
            }
            //inserting in buffer
            else {
                data = malloc(data_size);
                assert(data);
                check.value->address = data;
                buffer_counter++;
            }
        }
        // element is already in buffer
        else {
            hits++;
            if( i - check.value->last > Correleted_Reference_period) {
                correl_period_of_refd_page = check.value->value + check.value->last - check.value->hist1;
                check.value->hist1 = i;
                check.value->last = i;
                tree_update(t, check.value, correl_period_of_refd_page );
            }
            else
                check.value->last = i;
        }
    }
    free_hashtab(hashtab);
    tree_destroy(t);
    printf("%d", hits);
}
