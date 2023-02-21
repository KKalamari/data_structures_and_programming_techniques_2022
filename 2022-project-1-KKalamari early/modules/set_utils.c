#include "ADTSet.h"
#include <stdlib.h>
#include "common_types.h"
#include "set_utils.h"

Pointer set_find_eq_or_greater(Set set, Pointer value) {
    SetNode node;
    Pointer HasValue = set_find(set,value);
    Pointer next_value;
    Pointer new;
    if(!HasValue) {
        set_insert(set,value);
        new = set_find(set,value);
        node = set_find_node(set,value);
        node = set_next(set,node);
        if(node != NULL)
            next_value = set_node_value(set,node);
        set_remove(set,new);
        if (!next_value) {
            return NULL;
        }
        return next_value;
    }
    return value;
}


Pointer set_find_eq_or_smaller(Set set, Pointer value) {
    SetNode node;
    Pointer HasValue = set_find(set,value);
    Pointer next_value;
    Pointer new;
    if(!HasValue) {
        set_insert(set,value);
        new = set_find(set,value);
        node = set_find_node(set,value);
        node = set_previous(set,node);
        if(node != NULL)
            next_value = set_node_value(set,node);
        set_remove(set,new);
        if (!next_value) {
            return NULL;
        }
        return next_value;
    }
    return value;
}