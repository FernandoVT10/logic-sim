#include <stdlib.h>
#include <assert.h>

#include "raylib.h"
#include "components.h"
#include "actions.h"
#include "state.h"

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

void *alloc(size_t bytes) {
    void *ptr = calloc(1, bytes);
    assert(ptr != NULL && "Error allocating memory");
    return ptr;
}

static Component *component_new(ComponentType type, void *data) {
    Component *comp = alloc(sizeof(Component));
    comp->id = state.component_id++;
    comp->type = type;
    comp->data = data;
    return comp;
}

void add_component_at_start(ComponentType type, void *data) {
    Component *comp = component_new(type, data);

    if(state.components.count == 0) {
        state.components.head = comp;
        state.components.tail = comp;
    } else {
        comp->next = state.components.head;
        state.components.head = comp;
    }

    state.components.count++;
}

void add_component(ComponentType type, void *data) {
    Component *comp = component_new(type, data);

    if(state.components.count == 0) {
        state.components.head = comp;
    } else {
        state.components.tail->next = comp;
    }

    state.components.tail = comp;
    state.components.count++;
}

void delete_component(size_t id) {
    Component *comp = state.components.head;
    Component *prev_comp = NULL;
    size_t pos = 0;
    while(comp != NULL) {
        if(comp->id == id) {
            if(pos == 0) {
                state.components.head = comp->next;
            }

            if(pos == state.components.count - 1) {
                state.components.tail = prev_comp;
            }
            free(comp);
            state.components.count--;
            break;
        }
        pos++;
        prev_comp = comp;
        comp = comp->next;
    }
}

static Vector2 pin_get_pos(Pin pin) {
    return Vector2Add(*pin.parent_pos, pin.pos);
}

static void pin_update(Pin *pin) {
    if(CheckCollisionPointCircle(GetMousePosition(), pin_get_pos(*pin), PIN_RADIUS)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    ) {
        if(state.wiring) {
            wiring_end(pin);
        } else {
            wiring_start(pin);
        }
    }
}

static void pin_draw(Pin pin) {
    DrawCircleV(pin_get_pos(pin), PIN_RADIUS, COMP_COLOR);
}

Switch *switch_new(Vector2 initial_pos) {
    Switch *sw = alloc(sizeof(Switch));
    sw->pos = initial_pos;

    sw->pin.type = PIN_OUTPUT;
    sw->pin.parent_pos = &sw->pos;
    sw->pin.pos.x = SWITCH_DRAGGABLE_WIDTH + SWITCH_DRAGGABLE_MARGIN + SWITCH_WIDTH + SWITCH_LINE_WIDTH + PIN_RADIUS;
    sw->pin.pos.y = SWITCH_HEIGHT / 2;
    return sw;
}

void switch_update(Switch *sw) {
    pin_update(&sw->pin);

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
        sw->pin.on = !sw->pin.on;
    }
}

void switch_draw(Switch *sw) {
    Color color = sw->pin.on ? ACTIVE_COMP_COLOR : COMP_COLOR;
    float pos_x = sw->pos.x;

    // draggable rectangle before the switch
    // DrawRectangle(pos_x, sw->pos.y, SWITCH_DRAGGABLE_WIDTH, SWITCH_HEIGHT, FCOLOR);
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
    if(sw->pin.on) {
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

    pin_draw(sw->pin);
}

Nand *nand_new(Vector2 initial_pos) {
    Nand *nand = alloc(sizeof(Nand));
    nand->pos = initial_pos;

    nand->in[0] = (Pin){
        .type = PIN_INPUT,
        .pos = {0, PIN_RADIUS},
        .parent_pos = &nand->pos,
    };

    nand->in[1] = (Pin){
        .type = PIN_INPUT,
        .pos = {0, NAND_HEIGHT - PIN_RADIUS},
        .parent_pos = &nand->pos,
    };

    nand->out = (Pin){
        .type = PIN_OUTPUT,
        .pos = {NAND_WIDTH, NAND_HEIGHT/2},
        .parent_pos = &nand->pos,
    };

    return nand;
}

void nand_update(Nand *nand) {
    pin_update(&nand->out);
    pin_update(&nand->in[0]);
    pin_update(&nand->in[1]);
    nand->out.on = !(nand->in[0].on && nand->in[1].on);

    Rectangle collider = {
        .x = nand->pos.x,
        .y = nand->pos.y,
        .width = NAND_WIDTH,
        .height = NAND_HEIGHT,
    };

    if(CheckCollisionPointRec(GetMousePosition(), collider)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    ) {
        drag_start(&nand->pos);
    }
}

void nand_draw(Nand *nand) {
    pin_draw(nand->in[0]);
    pin_draw(nand->in[1]);
    pin_draw(nand->out);

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

Led *led_new(Vector2 initial_pos) {
    Led *led = alloc(sizeof(Led));
    led->pos = initial_pos;

    led->pin.type = PIN_INPUT;
    led->pin.parent_pos = &led->pos;
    led->pin.pos = (Vector2){2, LED_HEIGHT / 2};
    return led;
}

void led_update(Led *led) {
    pin_update(&led->pin);
    Rectangle collider = {
        .x = led->pos.x,
        .y = led->pos.y,
        .width = LED_WIDTH,
        .height = LED_HEIGHT,
    };

    if(CheckCollisionPointRec(GetMousePosition(), collider)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    ) {
        drag_start(&led->pos);
    }
}

void led_draw(Led *led) {
    pin_draw(led->pin);

    DrawRectangle(led->pos.x, led->pos.y, LED_WIDTH, LED_HEIGHT, COMP_COLOR);

    Color color = led->pin.on ? LED_ACTIVE_COLOR : LED_COLOR;

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

Wire *wire_new() {
    return alloc(sizeof(Wire));
}
void wire_update(Wire *wire) {
    if(wire->input != NULL) {
        wire->on = wire->input->on;
    }

    if(wire->out != NULL) {
        wire->out->on = wire->on;
    }
}

void wire_draw(Wire *wire) {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 start_pos = wire->input != NULL ? pin_get_pos(*wire->input) : mouse_pos;
    Vector2 end_pos = wire->out != NULL ? pin_get_pos(*wire->out) : mouse_pos;

    Color wire_color = wire->on ? WIRE_ON_COLOR : WIRE_OFF_COLOR;

    DrawLineEx(start_pos, end_pos, WIRE_THICKNESS, wire_color);
}
