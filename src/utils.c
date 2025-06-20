#include "utils.h"

void *alloc(size_t bytes) {
    void *ptr = calloc(1, bytes);
    assert(ptr != NULL && "Error allocating memory");
    return ptr;
}

static SetItem *item_new(void *data) {
    SetItem *item = alloc(sizeof(SetItem));
    item->data = data;
    return item;
}

Set *set_create() {
    return alloc(sizeof(Set));
}

void set_add(Set *set, void *data) {
    SetItem *item = item_new(data);

    if(set->count == 0) {
        set->head = set->tail = item;
    } else {
        // prev item
        set->tail->next = item;
        set->tail = item;
    }

    set->count++;
}

void set_delete(Set *set, void *data_ptr) {
    SetItem *item = NULL;
    SetItem *prev_item = NULL;
    size_t pos = 0;
    for(list_each(item, set)) {
        if(item->data == data_ptr) {
            if(set->count == 1) {
                set->tail = set->head = NULL;
            } else if(pos == 0) {
                set->head = item->next;
            } else if(pos == set->count - 1) {
                prev_item->next = NULL;
                set->tail = prev_item;
            } else {
                prev_item->next = item->next;
            }
            free(item);
            set->count--;
            break;
        }
        prev_item = item;
        pos++;
    }
}
