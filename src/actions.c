#include <stdio.h>
#include <stdlib.h>
#include "actions.h"
#include "components.h"
#include "state.h"
#include "utils.h"

void drag_start(Vector2 *pos) {
    if(state.action != ACTION_NONE) return;

    state.action = ACTION_DRAGGING;
    state.drag_pos = pos;
}

void drag_update() {
    if(state.action != ACTION_DRAGGING) return;

    *state.drag_pos = Vector2Add(*state.drag_pos, GetMouseDelta());

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        state.action = ACTION_NONE;
        state.drag_pos = NULL;
    }
}

void wiring_start(Pin *pin) {
    if(state.action != ACTION_NONE) return;

    state.action = ACTION_WIRING;

    state.cur_wire = wire_new();

    if(pin->type == PIN_INPUT) {
        state.cur_wire->right = pin;
    } else {
        state.cur_wire->on = pin->on;
        state.cur_wire->left = pin;
    }
}

void wiring_end(Pin *pin) {
    if(state.action != ACTION_WIRING && pin) return;

    Wire *wire = state.cur_wire;
    if(wire->left == NULL && wire->right == NULL) return;

    if(pin->type == PIN_INPUT && wire->right == NULL) {
        wire->right = pin;
    } else if(pin->type == PIN_OUTPUT && wire->left == NULL) {
        wire->left = pin;
    } else {
        return;
    }

    set_add(state.wires, wire);

    // adding references to the pins
    set_add(wire->left->wires, wire);
    set_add(wire->right->wires, wire);

    update_wire_state(wire, wire->left->on);

    state.action = ACTION_NONE;
}

void wiring_update() {
    if(state.action != ACTION_WIRING) return;

    wire_draw(state.cur_wire);

    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // cancel wiring
        state.action = ACTION_NONE;
        free(state.cur_wire);
        state.cur_wire = NULL;
    }
}
