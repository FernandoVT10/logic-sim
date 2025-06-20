#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stddef.h>
#include <stdbool.h>
#include "raymath.h"
#include "utils.h"

typedef struct Wire Wire;
typedef struct Component Component;

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    PinType type;
    bool on;
    Set *wires;
    Component *parent;
    Vector2 pos; // relative position based in the parent
} Pin;

// typedef struct {
//     bool on;
//     Set *wires;
//     Component *parent;
//     Vector2 *parent_pos;
//     Vector2 relative_pos;
// } PinInput;
//
// typedef struct {
//     bool on;
//     Set *wires;
//     Vector2 *parent_pos;
//     Vector2 relative_pos;
// } PinOutput;

typedef enum {
    SWITCH,
    LED,
    NAND_GATE,
    // WIRE,
} ComponentType;

typedef struct {
    Pin *items;
    size_t count;
    size_t capacity;
} Pins;

struct Component {
    ComponentType type;
    Vector2 pos;

    Pins inputs;
    Pins outputs;

    void (*draw)(Component*);
    void (*update)(Component*);
    void (*update_state)(Component*);
};

struct Wire {
    bool on;
    Pin *left;
    Pin *right;
};

void component_add(Component*);
void component_update_pins(Component*);
void component_draw_pins(Component*);

Component *switch_new(Vector2 initial_pos);
Component *nand_new(Vector2 initial_pos);
Component *led_new(Vector2 initial_pos);

Wire *wire_new();
void wire_draw(Wire* wire);
void update_wire_state(Wire* wire, bool on);
void wire_delete(Wire* wire);
void wire_update(Wire* wire);

#endif // COMPONENTS_H
