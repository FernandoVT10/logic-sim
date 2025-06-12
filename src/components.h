#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stddef.h>
#include <stdbool.h>
#include "raymath.h"

typedef struct Wire Wire;
typedef struct Component Component;

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    bool on;
    Component *parent;
} PinInput;

typedef struct {
    bool on;
    Wire *wire;
} PinOutput;

typedef enum {
    SWITCH,
    LED,
    NAND_GATE,
    // WIRE,
} ComponentType;

struct Component {
    ComponentType type;
    Vector2 pos;

    struct {
        PinInput *items;
        size_t count;
        size_t capacity;
    } inputs;

    struct {
        PinOutput *items;
        size_t count;
        size_t capacity;
    } outputs;

    void (*draw)(Component*);
    void (*update)(Component*);
    void (*update_state)(Component*);

    Component *next;
};

typedef struct {
    Component *head;
    Component *tail;
    size_t count;
} Components;

typedef struct {
    int type;
} Pin;

struct Wire {
    bool on;
    Pin *input;
    Pin *out;
    PinOutput *left;
    PinInput *right;
};

void add_component_at_start(Component*); // adds the component at the beginning of the list
void add_component(Component*); // adds the component at the end of the list
void delete_component(void *component_ptr);

Component *switch_new(Vector2 initial_pos);

// Nand *nand_new(Vector2 initial_pos);
// void nand_update(Nand *nand);
// void nand_draw(Nand *nand);

Component *led_new(Vector2 initial_pos);

// Wire *wire_new();
// void wire_delete(Wire *wire);
// void wire_update(Wire *wire);
// void wire_draw(Wire *wire);

#endif // COMPONENTS_H
