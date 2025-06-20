#include <stdlib.h>
#include <assert.h>

#include "raylib.h"
#include "components.h"
#include "actions.h"
#include "state.h"
#include "utils.h"

#define COMP_COLOR CLITERAL(Color){ 58, 62, 74, 255 }
#define ACTIVE_COMP_COLOR CLITERAL(Color){ 15, 182, 214, 255 }

#define PIN_RADIUS 8

#define SWITCH_WIDTH 30
#define SWITCH_HEIGHT 30
#define SWITCH_LINE_WIDTH 5 // line width that is drawn after the square
#define SWITCH_LINE_THICKNESS 5 // line thickness that is drawn after the square
#define SWITCH_DRAGGABLE_WIDTH 8 // Little square before the square of the switch used to drag it
#define SWITCH_DRAGGABLE_MARGIN 5 // margin between the draggable rectangle and the switch

#define NAND_WIDTH 100
#define NAND_HEIGHT 40
#define NAND_BG_COLOR CLITERAL(Color){ 191, 13, 78, 255 }

#define LED_WIDTH 24
#define LED_HEIGHT 24
#define LED_ACTIVE_COLOR CLITERAL(Color){ 190, 14, 70, 255 }
#define LED_COLOR CLITERAL(Color){ 95, 7, 48, 255 }

#define WIRE_THICKNESS 5
#define WIRE_ON_COLOR CLITERAL(Color){ 15, 182, 214, 255 }
#define WIRE_OFF_COLOR CLITERAL(Color){ 7, 86, 95, 255 }

static Component *component_new(ComponentType type) {
    Component *comp = alloc(sizeof(Component));
    comp->type = type;
    return comp;
}

static Pin create_pin(PinType type, Component *parent, Vector2 pos) {
    return (Pin) {
        .type = type,
        .pos = pos,
        .parent = parent,
        .wires = set_create(),
    };
}

static void pin_update(Pin *pin) {
    Vector2 pos = Vector2Add(pin->parent->pos, pin->pos);

    if(CheckCollisionPointCircle(GetMousePosition(), pos, PIN_RADIUS)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    ) {
        if(state.action == ACTION_WIRING) {
            wiring_end(pin);
        } else {
            wiring_start(pin);
        }
    }
}

static void pin_draw(Pin pin) {
    Vector2 pos = Vector2Add(pin.parent->pos, pin.pos);
    DrawCircleV(pos, PIN_RADIUS, COMP_COLOR);
}

static void pin_update_state(Pin *pin, bool state) {
    pin->on = state;

    if(pin->type == PIN_INPUT) {
        if(pin->parent->update_state) pin->parent->update_state(pin->parent);
    } else {
        SetItem *item = NULL;
        for(list_each(item, pin->wires)) {
            // weird syntax if not added
            {update_wire_state(item->data, state);}
        }
    }
}

void component_add(Component *comp) {
    set_add(state.components, comp);
}

void component_delete(Component *comp) {
    for(size_t i = 0; i < comp->inputs.count; i++) {
        Pin pin = comp->inputs.items[i];
        SetItem *item = pin.wires->head;
        SetItem *next_item;
        while(item != NULL) {
            next_item = item->next;
            wire_delete(item->data);
            item = next_item;
        }
    }

    for(size_t i = 0; i < comp->outputs.count; i++) {
        Pin pin = comp->outputs.items[i];
        SetItem *item = pin.wires->head;
        SetItem *next_item;
        while(item != NULL) {
            next_item = item->next;
            wire_delete(item->data);
            item = next_item;
        }
    }

    set_delete(state.components, comp);
    free(comp);
}

void component_update_pins(Component* comp) {
    for(size_t i = 0; i < comp->inputs.count; i++) {
        pin_update(&comp->inputs.items[i]);
    }

    for(size_t i = 0; i < comp->outputs.count; i++) {
        pin_update(&comp->outputs.items[i]);
    }
}

void component_draw_pins(Component* comp) {
    for(size_t i = 0; i < comp->inputs.count; i++) {
        pin_draw(comp->inputs.items[i]);
    }

    for(size_t i = 0; i < comp->outputs.count; i++) {
        pin_draw(comp->outputs.items[i]);
    }
}

static void switch_update(Component *sw) {
    Rectangle draggable_collider = {
        .x = sw->pos.x,
        .y = sw->pos.y,
        .width = SWITCH_DRAGGABLE_WIDTH,
        .height = SWITCH_HEIGHT,
    };

    if(CheckCollisionPointRec(GetMousePosition(), draggable_collider)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    ) {
        drag_start(&sw->pos);
    }

    Rectangle switch_collider = {
        .x = sw->pos.x + SWITCH_DRAGGABLE_WIDTH + SWITCH_DRAGGABLE_MARGIN,
        .y = sw->pos.y,
        .width = SWITCH_WIDTH,
        .height = SWITCH_HEIGHT,
    };
    bool collision = CheckCollisionPointRec(GetMousePosition(), switch_collider);
    if(collision && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool on = !sw->outputs.items[0].on;
        pin_update_state(&sw->outputs.items[0], on);
    }

    Rectangle collider = {
        .x = sw->pos.x,
        .y = sw->pos.y,
        .width = SWITCH_DRAGGABLE_WIDTH + SWITCH_DRAGGABLE_MARGIN + SWITCH_WIDTH,
        .height = SWITCH_HEIGHT,
    };
    if(CheckCollisionPointRec(GetMousePosition(), collider)
        && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
        && state.action == ACTION_NONE
    ) {
        component_delete(sw);
    }
}

static void switch_draw(Component *sw) {
    bool on = sw->outputs.items[0].on;
    Color color = on ? ACTIVE_COMP_COLOR : COMP_COLOR;
    float pos_x = sw->pos.x;

    // draggable rectangle before the switch
    Rectangle draggable_rec = {
        .x = pos_x,
        .y = sw->pos.y,
        .width = SWITCH_DRAGGABLE_WIDTH,
        .height = SWITCH_HEIGHT,
    };
    DrawRectangleRounded(draggable_rec, 1, 10, COMP_COLOR);
    pos_x += SWITCH_DRAGGABLE_WIDTH + SWITCH_DRAGGABLE_MARGIN;

    // Rectangle of the switch
    // DrawRectangleLines(pos_x, sw->pos.y, SWITCH_WIDTH, SWITCH_HEIGHT, color);
    Rectangle rec = {
        .x = pos_x,
        .y = sw->pos.y,
        .width = SWITCH_WIDTH,
        .height = SWITCH_HEIGHT,
    };
    DrawRectangleLinesEx(rec, 2, color);

    // indicator of the switch state
    if(on) {
        int inner_width = 20;
        int inner_height = 20;
        DrawRectangle(
            pos_x + SWITCH_WIDTH / 2 - inner_width / 2,
            sw->pos.y + SWITCH_HEIGHT / 2 - inner_height / 2,
            inner_width,
            inner_height,
            ACTIVE_COMP_COLOR
        );
    }
    pos_x += SWITCH_WIDTH;

    // line after the switch that connects to the pin
    Vector2 line_pos = {pos_x, sw->pos.y + SWITCH_HEIGHT / 2};
    Vector2 line_end = {line_pos.x + SWITCH_LINE_WIDTH, line_pos.y};
    DrawLineEx(line_pos, line_end, SWITCH_LINE_THICKNESS, COMP_COLOR);
}

Component *switch_new(Vector2 initial_pos) {
    Component *sw = component_new(SWITCH);
    sw->pos = initial_pos;
    sw->draw = &switch_draw;
    sw->update = &switch_update;

    Vector2 pos = {
        .x = SWITCH_DRAGGABLE_WIDTH + SWITCH_DRAGGABLE_MARGIN + SWITCH_WIDTH + SWITCH_LINE_WIDTH + PIN_RADIUS,
        .y = SWITCH_HEIGHT / 2,
    };
    da_append(&sw->outputs, create_pin(PIN_OUTPUT, sw, pos));
    return sw;
}

static void nand_draw(Component *nand) {
    DrawRectangle(nand->pos.x, nand->pos.y, NAND_WIDTH, NAND_HEIGHT, NAND_BG_COLOR);

    const char *text = "NAND";
    int font_size = 26;
    int text_width = MeasureText(text, font_size);

    DrawText(
        text,
        nand->pos.x + NAND_WIDTH / 2 - text_width / 2,
        nand->pos.y + NAND_HEIGHT / 2 - font_size / 2,
        font_size,
        WHITE
    );
}

static void nand_update(Component *nand) {
    Rectangle collider = {
        .x = nand->pos.x,
        .y = nand->pos.y,
        .width = NAND_WIDTH,
        .height = NAND_HEIGHT,
    };

    if(CheckCollisionPointRec(GetMousePosition(), collider)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drag_start(&nand->pos);
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && state.action == ACTION_NONE) {
            component_delete(nand);
        }
    }
}

static void nand_update_state(Component *nand) {
    bool on = !(nand->inputs.items[0].on && nand->inputs.items[1].on);
    pin_update_state(&nand->outputs.items[0], on);
}

Component *nand_new(Vector2 initial_pos) {
    Component *nand = component_new(NAND_GATE);
    nand->pos = initial_pos;
    nand->draw = &nand_draw;
    nand->update = &nand_update;
    nand->update_state = &nand_update_state;

    da_append(
        &nand->inputs,
        create_pin(PIN_INPUT, nand, (Vector2){0, PIN_RADIUS})
    );

    da_append(
        &nand->inputs,
        create_pin(PIN_INPUT, nand, (Vector2){0, NAND_HEIGHT - PIN_RADIUS})
    );

    da_append(
        &nand->outputs,
        create_pin(PIN_OUTPUT, nand, (Vector2){NAND_WIDTH, NAND_HEIGHT/2})
    );

    nand_update_state(nand);

    return nand;
}

static void led_draw(Component *led) {
    DrawRectangle(led->pos.x, led->pos.y, LED_WIDTH, LED_HEIGHT, COMP_COLOR);

    Color color = led->inputs.items[0].on ? LED_ACTIVE_COLOR : LED_COLOR;

    int inner_width = LED_WIDTH - 6;
    int inner_height = LED_HEIGHT - 6;
    DrawRectangle(
        led->pos.x + LED_WIDTH / 2 - inner_width / 2,
        led->pos.y + LED_HEIGHT / 2 - inner_height / 2,
        inner_width,
        inner_height,
        color
    );
}

static void led_update(Component *led) {
    Rectangle collider = {
        .x = led->pos.x,
        .y = led->pos.y,
        .width = LED_WIDTH,
        .height = LED_HEIGHT,
    };

    if(CheckCollisionPointRec(GetMousePosition(), collider)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drag_start(&led->pos);
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && state.action == ACTION_NONE) {
            component_delete(led);
        }
    }
}

Component *led_new(Vector2 initial_pos) {
    Component *led = component_new(LED);
    led->pos = initial_pos;
    led->draw = &led_draw;
    led->update = &led_update;

    da_append(
        &led->inputs,
        create_pin(PIN_INPUT, led, (Vector2){2, LED_HEIGHT/2})
    );

    return led;
}

void wire_draw(Wire *wire) {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 start_pos = wire->left != NULL ? Vector2Add(wire->left->parent->pos, wire->left->pos) : mouse_pos;
    Vector2 end_pos = wire->right != NULL ? Vector2Add(wire->right->parent->pos, wire->right->pos) : mouse_pos;

    Color wire_color = wire->on ? WIRE_ON_COLOR : WIRE_OFF_COLOR;

    DrawLineEx(start_pos, end_pos, WIRE_THICKNESS, wire_color);
}

Wire *wire_new() {
    return alloc(sizeof(Wire));
}

void update_wire_state(Wire *wire, bool on) {
    wire->on = on;
    pin_update_state(wire->right, on);
}

void wire_delete(Wire *wire) {
    if(wire->right) {
        pin_update_state(wire->right, false);
        set_delete(wire->right->wires, wire);
    }

    if(wire->left) {
        set_delete(wire->left->wires, wire);
    }

    set_delete(state.wires, wire);
    free(wire);
}

void wire_update(Wire *wire) {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 left_pos = Vector2Add(wire->left->parent->pos, wire->left->pos);
    Vector2 right_pos = Vector2Add(wire->right->parent->pos, wire->right->pos);

    if(CheckCollisionPointLine(mouse_pos, left_pos, right_pos, WIRE_THICKNESS)
        && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
        && state.action == ACTION_NONE
    ) {
        wire_delete(wire);
    }
}
