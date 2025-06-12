#include <stdio.h>
#include <stdlib.h>
#include "actions.h"
#include "components.h"
#include "state.h"

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
    if(state.action != ACTION_NONE && pin) return;

    state.action = ACTION_WIRING;

    // Wire *wire = wire_new(0);
    //
    // if(pin->type == PIN_INPUT) {
    //     wire->out = pin;
    // } else {
    //     wire->input = pin;
    // }
    //
    // state.cur_wire = wire;
}

void wiring_end(Pin *pin) {
    if(state.action != ACTION_WIRING) return;

    Wire *wire = state.cur_wire;
    if(wire->input == NULL && wire->out == NULL) return;

    int type = wire->input != NULL
        ? wire->input->type
        : wire->out->type;
    // if they are the same time we omit this call
    if(pin->type == type) return;

    if(pin->type == PIN_INPUT) {
        state.cur_wire->out = pin;
    } else {
        state.cur_wire->input = pin;
    }

    // little hack to draw the wire behind all components :)
    // add_component_at_start(COMP_WIRE, wire);

    // adding references to the pins
    // wire->input->wire = wire;
    // wire->out->wire = wire;

    state.cur_wire = NULL;
    state.action = ACTION_NONE;
}

void wiring_update() {
    if(state.action != ACTION_WIRING) return;

    // wire_draw(state.cur_wire);

    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // cancel wiring
        free(state.cur_wire);
        state.action = ACTION_NONE;
        state.cur_wire = NULL;
    }
}
