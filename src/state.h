#ifndef STATE_H
#define STATE_H

#include "components.h"

typedef struct {
    struct {
        bool dragging;
        Vector2 *pos; // pointer used to update the position when dragging
    } drag;

    bool dragging;
    Vector2 *drag_pos;

    Wire *cur_wire;
    bool wiring;

    size_t component_id;
    Components components;
} State;

extern State state;

#endif // STATE_H
