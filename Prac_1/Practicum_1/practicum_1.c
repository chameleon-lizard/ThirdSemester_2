#include <stdlib.h>
#include <stdio.h>

typedef struct LinkedList {
    int data;
    int key;
    struct LinkedList* next;
} ListItem;

enum
{
    ERR_OUTOFBOUNDS = -1,
    SUCCESS = 0
};

int 
print(ListItem *list) {
    ListItem *current = list;

    while (current != 0) {
        printf("list[%d] = %d;\n", current->key, current->data);
        current = current->next;
    }

    return SUCCESS;
}

int
add_last(ListItem *list, int info) {
    // Getting to the end of the list:
    int number = 1;
    ListItem *current = list;
    while (current->next != 0) {
        current = current->next;
        number++;
    }

    // Adding a new item:
    current->next = (struct LinkedList *)calloc(1, sizeof(ListItem));
    current->next->data = info;
    current->next->key = number;
    current->next->next = 0;
    return current->next->key;
}

int
add_first(ListItem **list, int info) {
    ListItem *buffer = (struct LinkedList *)calloc(1, sizeof(ListItem));
    buffer->data = info;
    buffer->key = 0;
    buffer->next = *list;

    ListItem *iterator = buffer->next;
    while (iterator != 0) {
        iterator->key++;
        iterator = iterator->next;
    }

    *list = buffer;
    return 0;
}

int 
rm_first(ListItem **list) {

    // If the list has no items, someone is dumb, so returning out of bounds:
    if (*list == 0) {
        return ERR_OUTOFBOUNDS;
    }

    // Else, we just create an item that points to the next item in the list and delete it:
    ListItem *current = (*list)->next;
    free(*list);
    *list = current;

    // Now we correct the keys of all the items:
    while (current != 0) {
        current->key--;
        current = current->next;
    }

    return SUCCESS;
}

int 
rm_last(ListItem *list) {
    // If we have a single item:
    if (list->next == 0) {
        free(list);
        return SUCCESS;
    }

    // Finding the previous to the needed item:
    ListItem *current = list;
    while (current->next->next != 0) {
        current = current->next;
    }

    // Deleting the last item:
    free(current->next);
    current->next = 0;
    return SUCCESS;
}

int 
rm(ListItem **list, int n, int size) 
{
    // Checking if we need to remove first or last item from the list:
    if (size == n) {
        rm_last(*list);
        return SUCCESS;
    } else if (0 == n) {
        rm_first(list);
        return SUCCESS;
    }

    ListItem *current = *list;

    // Finding the item that we need to delete:
    while (current->next->key != n) {
        if (current->next == 0) {
            return ERR_OUTOFBOUNDS;
        }

        current = current->next;
    }

    ListItem *buffer = current->next;
    current->next = current->next->next;
    free(buffer);

    // Now we correct the keys of all the items:
    current = current->next;
    while (current != 0) {
        current->key--;
        current = current->next;
    }

    return n;
}
int
add(ListItem **list, int n, int info, int size)
{
    // Checking if we need to add a first or last item to the list:
    if (size == 0) {
        add_first(list, info);
        return size + 1;
    } else if (size == n) {
        add_last(*list, info);
        return size + 1;
    } else if (0 == n) {
        add_first(list, info);
        return size + 1;
    } else if (n > size) {
        return ERR_OUTOFBOUNDS;
    }

    ListItem *current = *list;
    ListItem *previous = (ListItem *)-1;

    // Finding the iten with the correct key:
    while (current != 0 && current->key != n) {
        previous = current;
        current = current->next;
    }

    // Now we have two pointers - to the previous and to the next. Let's add 
    // the item between them:
    previous->next = (ListItem *)calloc(1, sizeof(ListItem));
    previous->next->data = info;
    previous->next->key = previous->key + 1;
    previous->next->next = current;

    // Now we have to correct the keys:
    while (current != 0) {
        current->key++;
        current = current->next;
    }

    return size + 1;
}

ListItem
get(ListItem **list, int n, int size) {
    if (n <= size) {
        ListItem *current = *list;
        while (current->key != n) {
            current = current->next;
        }

        return *current;
    } else {
        printf("OUB. Returning the first item.\n");
        return **list;
    }
}

int
swap(ListItem **list, int a, int b, int size)
{
    if (a > size || b > size) {
        return ERR_OUTOFBOUNDS;
    }

    ListItem a_buffer = get(list, a, size);
    ListItem b_buffer = get(list, b, size);

    rm(list, a, size);
    add(list, a, b_buffer.data, size);
    rm(list, b, size);
    add(list, b, a_buffer.data, size);

    return SUCCESS;
}

int
sort(ListItem **list, int size)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (get(list, i, size).data >= get(list, j, size).data) {
                swap(list, i, j, size);
            }
        }
    }

    return SUCCESS;
}

// TODO: Add sorting function.
// TODO: Testing :/

int 
main(void) {

    ListItem *list = 0;
    
    int item = -1;
    int size = 0;

    while (scanf("%d", &item) >= 0) {
        add(&list, size, item, size);
        size++;
    }

    sort(&list, size);

    print(list);

    return 0;
}

