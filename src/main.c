#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>

#include "raylib.h"
#include "raymath.h"
#include "components.h"
#include "state.h"
#include "actions.h"
#include "utils.h"

#define BG_COLOR CLITERAL(Color){ 16, 14, 23, 255 }

#define WIRE_THICKNESS 5
#define WIRE_ON_COLOR CLITERAL(Color){ 15, 182, 214, 255 }
#define WIRE_OFF_COLOR CLITERAL(Color){ 7, 86, 95, 255 }

State state = {0};

void wire_draw(Wire wire) {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 start_pos = wire.left != NULL ? Vector2Add(*wire.left->parent_pos, wire.left->relative_pos) : mouse_pos;
    Vector2 end_pos = wire.right != NULL ? Vector2Add(*wire.right->parent_pos, wire.right->relative_pos) : mouse_pos;

    Color wire_color = wire.on ? WIRE_ON_COLOR : WIRE_OFF_COLOR;

    DrawLineEx(start_pos, end_pos, WIRE_THICKNESS, wire_color);
}

int main() {
    InitWindow(1280, 720, "Logic Sim");
    SetTargetFPS(60);
    Component *sw1 = switch_new((Vector2){100, 100});
    add_component(sw1);

    Component *sw2 = switch_new((Vector2){100, 150});
    add_component(sw2);

    Component *nand = nand_new((Vector2){200, 100});
    add_component(nand);

    Component *led = led_new((Vector2){350, 100});
    add_component(led);

    da_append(&state.wires, ((Wire) {
        .left = &sw1->outputs.items[0],
        .right = &nand->inputs.items[0],
    }));
    da_append(&state.wires, ((Wire) {
        .left = &sw2->outputs.items[0],
        .right = &nand->inputs.items[1],
    }));
    da_append(&state.wires, ((Wire) {
        .left = &nand->outputs.items[0],
        .right = &led->inputs.items[0],
    }));

    sw1->outputs.items[0].wire = &state.wires.items[0];
    sw2->outputs.items[0].wire = &state.wires.items[1];
    nand->outputs.items[0].wire = &state.wires.items[2];

    nand->update_state(nand);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        if(IsKeyPressed(KEY_S)) {
            add_component(switch_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_L)) {
            add_component(led_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_N)) {
            add_component(nand_new(GetMousePosition()));
        }

        drag_update();
        wiring_update();

        // first update all components
        Component *comp;
        for(list_each(comp, &state.components)) {
            if(comp->update) comp->update(comp);
        }

        for(size_t i = 0; i < state.wires.count; i++) {
            Wire wire = state.wires.items[i];
            wire_draw(wire);
        }

        // then we draw them
        for(list_each(comp, &state.components)) {
            component_draw_pins(comp);
            // Wierd syntax if braces are removed
            {if(comp->draw) comp->draw(comp);}
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
