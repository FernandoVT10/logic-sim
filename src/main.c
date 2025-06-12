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
    Component *sw = switch_new((Vector2){100, 100});
    add_component(sw);

    Component *led = led_new((Vector2){200, 100});
    add_component(led);

    Wire wire = {
        .left = &sw->outputs.items[0],
        .right = &led->inputs.items[0],
    };

    sw->outputs.items[0].wire = &wire;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        if(IsKeyPressed(KEY_S)) {
            add_component(switch_new(GetMousePosition()));
        }

        if(IsKeyPressed(KEY_L)) {
            add_component(led_new(GetMousePosition()));
        }
        //
        // if(IsKeyPressed(KEY_N)) {
        //     add_component(COMP_NAND, nand_new(GetMousePosition()));
        // }

        drag_update();
        wiring_update();


        // first update all components
        Component *comp;
        for(list_each(comp, &state.components)) {
            if(comp->update) comp->update(comp);
        }

        // then we draw them
        for(list_each(comp, &state.components)) {
            if(comp->draw) comp->draw(comp);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
