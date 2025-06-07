#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stddef.h>
#include <stdbool.h>
#include "raymath.h"

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    PinType type;
    Vector2 pos;
    Vector2 *parent_pos;
    bool on;
} Pin;

typedef enum {
    COMP_SWITCH,
    COMP_LED,
    COMP_NAND,
    COMP_WIRE,
} ComponentType;

typedef struct Component Component;

struct Component {
    size_t id;
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
    bool on;
    Pin *input;
    Pin *out;
} Wire;

void add_component_at_start(ComponentType type, void *data); // adds the component at the beginning of the list
void add_component(ComponentType type, void *data); // adds the component at the end of the list
void delete_component(size_t id);

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
void wire_update(Wire *wire);
void wire_draw(Wire *wire);

#endif // COMPONENTS_H
