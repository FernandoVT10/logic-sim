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

#define PIN_RADIUS 8

State state = {0};

int main() {
    InitWindow(1280, 720, "Logic Sim");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        if(IsKeyPressed(KEY_S)) {
            add_component(COMP_SWITCH, switch_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_L)) {
            add_component(COMP_LED, led_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_N)) {
            add_component(COMP_NAND, nand_new(GetMousePosition()));
        }

        drag_update();
        wiring_update();

        // first update all components
        Component *comp;
        for(list_each(comp, &state.components)) {
            switch(comp->type) {
                case COMP_SWITCH: switch_update(comp->data); break;
                case COMP_NAND: nand_update(comp->data); break;
                case COMP_LED: led_update(comp->data); break;
                case COMP_WIRE: wire_update(comp->data); break;
            }
        }

        // then we draw them
        for(list_each(comp, &state.components)) {
            switch(comp->type) {
                case COMP_SWITCH: switch_draw(comp->data); break;
                case COMP_NAND: nand_draw(comp->data); break;
                case COMP_LED: led_draw(comp->data); break;
                case COMP_WIRE: wire_draw(comp->data); break;
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
