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

State state = {0};

int main() {
    state.components = set_create();
    state.wires = set_create();
    InitWindow(1280, 720, "Logic Sim");
    SetTargetFPS(60);
    // Component *sw1 = switch_new((Vector2){100, 100});
    // add_component(sw1);
    //
    // Component *sw2 = switch_new((Vector2){100, 150});
    // add_component(sw2);
    //
    // Component *nand = nand_new((Vector2){200, 100});
    // add_component(nand);
    //
    // Component *led = led_new((Vector2){350, 100});
    // add_component(led);
    //
    // da_append(&state.wires, ((Wire) {
    //     .left = &sw1->outputs.items[0],
    //     .right = &nand->inputs.items[0],
    // }));
    // da_append(&state.wires, ((Wire) {
    //     .left = &sw2->outputs.items[0],
    //     .right = &nand->inputs.items[1],
    // }));
    // da_append(&state.wires, ((Wire) {
    //     .left = &nand->outputs.items[0],
    //     .right = &led->inputs.items[0],
    // }));
    //
    // sw1->outputs.items[0].wire = &state.wires.items[0];
    // sw2->outputs.items[0].wire = &state.wires.items[1];
    // nand->outputs.items[0].wire = &state.wires.items[2];
    //
    // nand->update_state(nand);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        if(IsKeyPressed(KEY_S)) {
            component_add(switch_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_L)) {
            component_add(led_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_N)) {
            component_add(nand_new(GetMousePosition()));
        }

        drag_update();
        wiring_update();

        // first update all components
        SetItem *item = state.components->head;
        while(item != NULL) {
            SetItem *next_item = item->next;
            Component *comp = item->data;
            if(comp->update) comp->update(comp);
            item = next_item;
        }

        for(list_each(item, state.wires)) {
            Wire *wire = item->data;
            wire_draw(wire);
            wire_update(wire);
        }

        // then we draw them
        for(list_each(item, state.components)) {
            Component *comp = item->data;
            component_update_pins(comp);
            component_draw_pins(comp);
            // Wierd syntax if braces are removed
            {if(comp->draw) comp->draw(comp);}
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
