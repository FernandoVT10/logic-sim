#ifndef STATE_H
#define STATE_H

#include "components.h"
#include "utils.h"

typedef enum {
    ACTION_NONE,
    ACTION_WIRING,
    ACTION_DRAGGING,
} Action;

typedef struct {
    struct {
        bool dragging;
        Vector2 *pos; // pointer used to update the position when dragging
    } drag;

    Vector2 *drag_pos;

    Set *wires;
    Wire *cur_wire;

    Set *components;

    Action action;
} State;

extern State state;

#endif // STATE_H
