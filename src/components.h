#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stddef.h>
#include <stdbool.h>
#include "raymath.h"

typedef enum {
    NAND_GATE,
    LED,
    SWTICH,
} CompType;

typedef struct {
    CompType type;
    Pin inputs[MAX_INPUTS];
    Pin output[MAX_OUTPUTS];
    void (*update)(Comp*);
} Comp;

typedef struct Wire Wire;

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    PinType type;
    Vector2 pos;
    Vector2 relative_pos;
    bool on;
    Wire *wire;
} Pin;

typedef enum {
    COMP_SWITCH,
    COMP_LED,
    COMP_NAND,
    COMP_WIRE,
} ComponentType;

typedef struct Component Component;

struct Component {
    ComponentType type;
    void *data;
    Component *next;
};

typedef struct {
    Component *head;
    Component *tail;
    size_t count;
} Components;

typedef struct {
    Vector2 pos;
    Pin pin;
} Switch;

typedef struct {
    Vector2 pos;
    Pin in[2];
    Pin out;
} Nand;

typedef struct {
    Vector2 pos;
    Pin pin;
} Led;

typedef struct {
    Vector2 pos;
} Bridge;

struct Wire {
    bool on;
    Pin *input;
    Pin *out;
};

void add_component_at_start(ComponentType type, void *data); // adds the component at the beginning of the list
void add_component(ComponentType type, void *data); // adds the component at the end of the list
void delete_component(void *component_ptr);

Switch *switch_new(Vector2 initial_pos);
void switch_update(Switch *sw);
void switch_draw(Switch *sw);

Nand *nand_new(Vector2 initial_pos);
void nand_update(Nand *nand);
void nand_draw(Nand *nand);

Led *led_new(Vector2 initial_pos);
void led_update(Led *led);
void led_draw(Led *led);

Wire *wire_new();
void wire_delete(Wire *wire);
void wire_update(Wire *wire);
void wire_draw(Wire *wire);

#endif // COMPONENTS_H
