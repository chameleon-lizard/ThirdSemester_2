#include <stdlib.h>
#include <stdio.h>
#include "list.h"

int 
main(void) {

    ListItem *list = 0;
    
    int size = populate(&list);
    sort(&list, size);
    print(list);
    del(&list, size);

    return 0;
}

