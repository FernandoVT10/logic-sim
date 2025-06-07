#include <stdlib.h>
#include "actions.h"
#include "components.h"
#include "state.h"

void drag_start(Vector2 *pos) {
    if(state.dragging || state.wiring) return;

    state.dragging = true;
    state.drag_pos = pos;
}

void drag_update() {
    if(!state.dragging) return;

    *state.drag_pos = Vector2Add(*state.drag_pos, GetMouseDelta());

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        state.dragging = false;
        state.drag_pos = NULL;
    }
}

void wiring_start(Pin *pin) {
    state.wiring = true;

    Wire *wire = wire_new();

    if(pin->type == PIN_INPUT) {
        wire->out = pin;
    } else {
        wire->input = pin;
    }

    state.cur_wire = wire;

}

void wiring_end(Pin *pin) {
    Wire *wire = state.cur_wire;
    if(wire->input == NULL && wire->input == NULL) return;

    PinType type = wire->input != NULL
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
    add_component_at_start(COMP_WIRE, wire);

    state.cur_wire = NULL;
    state.wiring = false;
}

void wiring_update() {
    if(!state.wiring) return;

    wire_draw(state.cur_wire);

    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // cancel wiring
        free(state.cur_wire);
        state.wiring = false;
        state.cur_wire = NULL;
    }
}
