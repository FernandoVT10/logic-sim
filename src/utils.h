#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stdlib.h>

#define DA_INIT_CAP 16

#define da_append(da, item)                                                          \
    do {                                                                             \
        if((da)->count >= (da)->capacity) {                                          \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;   \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "No enough ram");                          \
        }                                                                            \
                                                                                     \
        (da)->items[(da)->count++] = (item);                                         \
    } while(0)

#define da_free(da) do { free((da)->items); } while(0)

#define list_each(item, list) \
    (item) = (list)->head; item != NULL; (item) = (item)->next

typedef struct SetItem SetItem;

struct SetItem {
    void *data;
    SetItem *next;
};

typedef struct {
    SetItem *head;
    SetItem *tail;
    size_t count;
} Set;

void *alloc(size_t bytes);

Set *set_create();
void set_add(Set *set, void *data);
void set_delete(Set *set, void *data_ptr);

#endif // UTILS_H
