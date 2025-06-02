#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "raylib.h"
#include "raymath.h"

#define BG_COLOR BLACK

#define DA_INIT_CAP 128

#define SWITCH_LINE_WIDTH 5
#define SWITCH_SIZE 30

#define LED_SIZE 30

#define PIN_RADIUS 8

#define da_append(da, item)                                                          \
    do {                                                                             \
        if((da)->count >= (da)->capacity) {                                          \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;   \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "No enough ram");                          \
        }                                                                            \
                                                                                     \
        (da)->items[(da)->count++] = (item);                                         \
    } while(0)

#define da_free(da) do { free((da)->items); } while(0)

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    PinType type;
    Vector2 pos;
    bool on;
} Pin;

typedef struct {
    Vector2 *items;
    size_t count;
    size_t capacity;
} Points;

typedef struct {
    Points points;
    bool on;
    Pin* pins[2];
} Wire;

typedef struct {
    Wire *items;
    size_t count;
    size_t capacity;
} Wires;

typedef struct {
    Vector2 pos;
    Pin pin;
} Switch;

typedef struct {
    Vector2 pos;
    bool in[2];
    bool out;
    bool holded;
} AndGate;

typedef struct {
    Vector2 pos;
    Pin pin;
    bool on;
} Led;

bool wiring = false;
Wires wires = {0};
Wire *cur_wire = NULL;

void draw_and_gate(AndGate gate) {
    int width = 100;
    int height = 50;

    int radius = 8;
    DrawCircle(gate.pos.x, gate.pos.y + 11 + 2, radius, GRAY);
    DrawCircle(gate.pos.x, gate.pos.y + 30 + 6, radius, GRAY);

    DrawRectangle(gate.pos.x, gate.pos.y, width, height, RED);

    const char *text = "AND";
    int font_size = 26;

    int text_width = MeasureText(text, font_size);
    DrawText(
        text,
        gate.pos.x + width / 2 - text_width / 2,
        gate.pos.y + height / 2 - font_size / 2,
        font_size,
        WHITE
    );
}

void update_and_gate(AndGate *gate) {
    int width = 100;
    int height = 40;
    Rectangle gate_rect = {
        .x = gate->pos.x,
        .y = gate->pos.y,
        .width = width,
        .height = height,
    };

    Vector2 mouse_pos = GetMousePosition();

    bool collision = CheckCollisionPointRec(mouse_pos, gate_rect);

    if(collision) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    if(!gate->holded && collision && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        gate->holded = true;
    }

    if(gate->holded && IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        gate->holded = false;
    }

    if(gate->holded) {
        gate->pos = Vector2Add(gate->pos, GetMouseDelta());
    }
}

bool can_wire_this_pin(Pin *pin) {
    // an input cannot connect to other input
    // and an ouput can't connect to other output
    return cur_wire->pins[0]->type != pin->type;
}

void handle_pin(Pin *pin) {
    int radius = PIN_RADIUS;

    Vector2 center = {
        .x = pin->pos.x + radius,
        .y = pin->pos.y
    };
    DrawCircleV(center, radius, GRAY);

    bool collision = CheckCollisionPointCircle(GetMousePosition(), center, radius);
    if(collision && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if(wiring) {
            if(can_wire_this_pin(pin)) {
                cur_wire->pins[1] = pin;
                wiring = false;
            }
        } else {
            wiring = true;
            da_append(&wires, ((Wire){
                .pins = {pin, NULL},
            }));
            cur_wire = &wires.items[wires.count - 1];
        }
    }
}

Switch *switch_new(Vector2 pos) {
    Switch *sw = malloc(sizeof(Switch));
    sw->pos = pos;
    sw->pin.type = PIN_OUTPUT;
    sw->pin.pos = (Vector2){pos.x + SWITCH_SIZE + SWITCH_LINE_WIDTH, pos.y + SWITCH_SIZE / 2};
    sw->pin.on = false;
    return sw;
}

void handle_switch(Switch *sw) {
    Color color = sw->pin.on ? BLUE : GRAY;

    Rectangle rec = {
        .x = sw->pos.x,
        .y = sw->pos.y,
        .width = SWITCH_SIZE,
        .height = SWITCH_SIZE,
    };
    float line_thickness = 2;
    DrawRectangleLinesEx(rec, line_thickness, color);

    Vector2 line_pos = {rec.x + rec.width, rec.y + rec.height / 2};
    Vector2 line_end = {line_pos.x + SWITCH_LINE_WIDTH, line_pos.y};
    DrawLineEx(line_pos, line_end, 5, GRAY);

    if(sw->pin.on) {
        int inner_size = 20;
        int diff = SWITCH_SIZE / 2 - inner_size / 2;
        DrawRectangle(rec.x + diff, rec.y + diff, inner_size, inner_size, BLUE);
    }

    bool collision = CheckCollisionPointRec(GetMousePosition(), rec);

    if(collision && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        sw->pin.on = !sw->pin.on;
    }

    handle_pin(&sw->pin);


    // Wire wire = sw->socket.wire;
    // if(wire.points.count > 1) {
    //     Color wire_color;
    //
    //     if(wire.on) {
    //         wire_color = BLUE;
    //     } else {
    //         wire_color = GRAY;
    //     }
    //
    //     for(size_t i = 0; i < wire.points.count - 1; i++) {
    //         Vector2 pA = wire.points.items[i];
    //         Vector2 pB = wire.points.items[i + 1];
    //         DrawLineEx(pA, pB, 5, wire_color);
    //     }
    // }

    // if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //     if(wire.points.count == 0) {
    //         da_append(&wire.points, GetMousePosition());
    //     }
    //
    //     da_append(&wire.points, GetMousePosition());
    //     cur_point = &wire.points.items[wire.points.count - 1];
    // }
    //
    // if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && cur_point != NULL) {
    //     *cur_point = GetMousePosition();
    // }
    //
    // if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    //     cur_point = NULL;
    // }
}

void handle_wires() {
    int thickness = 5;

    for(size_t i = 0; i < wires.count; i++) {
        Wire wire = wires.items[i];
        if(wire.pins[1] == NULL) continue;

        Vector2 start_pos = wire.pins[0]->pos;
        start_pos.x += PIN_RADIUS;

        Vector2 end_pos = wire.pins[1]->pos;
        end_pos.x += PIN_RADIUS;
        DrawLineEx(start_pos, end_pos, thickness, GRAY);
    }

    if(wiring && cur_wire != NULL && cur_wire->pins[0] != NULL) {
        Vector2 pos = cur_wire->pins[0]->pos;
        pos.x += PIN_RADIUS;
        DrawLineEx(pos, GetMousePosition(), thickness, GRAY);
    }

    if(wiring && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        wiring = false;
        wires.count--;
    }
    // TODO: add a cancel wiring function

    if(!wiring && cur_wire != NULL) {
        cur_wire = NULL;
    }
}

Led *led_new(Vector2 pos) {
    Led *led = malloc(sizeof(Led));
    led->pos = pos;
    led->on = false;

    led->pin.type = PIN_INPUT;
    led->pin.pos = (Vector2){led->pos.x - PIN_RADIUS, led->pos.y + LED_SIZE / 2};
    led->pin.on = false;
    return led;
}

void led_update(Led *led) {
    handle_pin(&led->pin);

    int size = LED_SIZE;
    DrawRectangle(led->pos.x, led->pos.y, size, size, GRAY);

    int inner_size = LED_SIZE - 10;
    int diff = size / 2 - inner_size / 2;
    DrawRectangle(led->pos.x + diff, led->pos.y + diff, inner_size, inner_size, RED);

    // Rectangle inner_rec = {
    //     .x = led->pos.x
    // };
    // DrawRectangleRec(rec, GRAY);
}

int main() {
    InitWindow(1280, 720, "Logic Sim");

    // AndGate gate = {
    //     .pos = {250, 70},
    // };

    Switch *sw1 = switch_new((Vector2){50, 50});
    Switch *sw2 = switch_new((Vector2){50, 100});

    Led *led1 = led_new((Vector2){250, 70});
    Led *led2 = led_new((Vector2){250, 140});

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        handle_switch(sw1);
        handle_switch(sw2);

        handle_wires();
        led_update(led1);
        led_update(led2);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
