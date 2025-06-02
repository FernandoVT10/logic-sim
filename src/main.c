#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>

#include "raylib.h"
#include "raymath.h"

#define SWITCH_LINE_WIDTH 5
#define SWITCH_SIZE 30

#define LED_SIZE 30

#define PIN_RADIUS 8


#define DA_INIT_CAP 128

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

typedef struct Wire Wire;

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT,
} PinType;

typedef struct {
    PinType type;
    Vector2 pos;
    Vector2 *parent_pos;
    bool on;
} Pin;

typedef struct {
    Vector2 pos;
    Pin pin;
} Switch;

typedef struct {
    Vector2 pos;
    Pin pin;
} Led;

struct Wire {
    bool on;
    Pin *input;
    Pin *out;
};

typedef struct {
    Wire *items;
    size_t count;
    size_t capacity;
} Wires;

typedef struct {
    Vector2 pos;
    Pin in[2];
    Pin out;
    bool holded;
} Nand;

Wires wires = {0};

Switch *switch_new(Vector2 pos) {
    Switch *sw = malloc(sizeof(Switch));
    bzero(sw, sizeof(Switch));
    sw->pos = pos;

    sw->pin.type = PIN_OUTPUT;
    sw->pin.pos = (Vector2){pos.x + SWITCH_SIZE + SWITCH_LINE_WIDTH, pos.y + SWITCH_SIZE / 2};
    return sw;
}

Led *led_new(Vector2 pos) {
    Led *led = malloc(sizeof(Led));
    bzero(led, sizeof(Led));
    led->pos = pos;

    led->pin.type = PIN_INPUT;
    led->pin.pos = (Vector2){led->pos.x - PIN_RADIUS, led->pos.y + LED_SIZE / 2};
    return led;
}

Nand *nand_new(Vector2 pos) {
    Nand *nand = malloc(sizeof(Nand));
    bzero(nand, sizeof(Nand));
    nand->pos = pos;

    nand->in[0] = (Pin){
        .type = PIN_INPUT,
        .pos = {0, 0},
        .parent_pos = &nand->pos,
    };

    nand->in[1] = (Pin){
        .type = PIN_INPUT,
        .pos = {0, 50},
        .parent_pos = &nand->pos,
    };

    nand->out = (Pin){
        .type = PIN_OUTPUT,
        .pos = {100, 25},
        .parent_pos = &nand->pos,
    };

    return nand;
}

Vector2 pin_get_pos(Pin *pin) {
    if(pin->parent_pos != NULL) {
        return Vector2Add(pin->pos, *pin->parent_pos);
    }

    return pin->pos;
}

void pin_draw(Pin *pin) {
    int radius = PIN_RADIUS;

    DrawCircleV(pin_get_pos(pin), radius, GRAY);
}

void switch_update(Switch *sw) {
    pin_draw(&sw->pin);

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
}

void led_update(Led *led) {
    pin_draw(&led->pin);

    int size = LED_SIZE;
    DrawRectangle(led->pos.x, led->pos.y, size, size, GRAY);

    Color color = led->pin.on ? RED : BLACK;

    int inner_size = LED_SIZE - 10;
    int diff = size / 2 - inner_size / 2;
    DrawRectangle(led->pos.x + diff, led->pos.y + diff, inner_size, inner_size, color);
}

void draw_wires() {
    int thickness = 5;

    for(size_t i = 0; i < wires.count; i++) {
        Wire wire = wires.items[i];
        wires.items[i].on = wire.input->on;

        if(wire.out == NULL) continue;
        wire.out->on = wire.input->on;

        Color wire_color;
        if(wire.on) {
            wire_color = (Color){124, 158, 232, 255};
        } else {
            wire_color = (Color){75, 95, 139, 255};
        }

        Vector2 start_pos = pin_get_pos(wire.input);
        Vector2 end_pos = pin_get_pos(wire.out);
        DrawLineEx(start_pos, end_pos, thickness, wire_color);
    }
}

void nand_update(Nand *nand) {
    int width = 100;
    int height = 50;

    pin_draw(&nand->in[0]);
    pin_draw(&nand->in[1]);
    pin_draw(&nand->out);

    nand->out.on = !(nand->in[0].on && nand->in[1].on);

    Rectangle rec = {
        .x = nand->pos.x,
        .y = nand->pos.y,
        .width = width,
        .height = height,
    };

    if(CheckCollisionPointRec(GetMousePosition(), rec)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        && !nand->holded
    ) {
        nand->holded = true;
    }

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && nand->holded) {
        nand->holded = false;
    }

    if(nand->holded) {
        nand->pos = Vector2Add(nand->pos, GetMouseDelta());
    }

    DrawRectangle(nand->pos.x, nand->pos.y, width, height, RED);

    const char *text = "NAND";
    int font_size = 26;

    int text_width = MeasureText(text, font_size);
    DrawText(
        text,
        nand->pos.x + width / 2 - text_width / 2,
        nand->pos.y + height / 2 - font_size / 2,
        font_size,
        WHITE
    );
}

int main() {
    InitWindow(1280, 720, "Logic Sim");

    Switch *sw1 = switch_new((Vector2){50, 25});
    Switch *sw2 = switch_new((Vector2){50, 75});


    Nand *nand = nand_new((Vector2){250, 50});

    da_append(&wires, ((Wire) {
        .input = &sw1->pin,
        .out = &nand->in[0],
    }));

    da_append(&wires, ((Wire) {
        .input = &sw2->pin,
        .out = &nand->in[1],
    }));

    Led *led = led_new((Vector2){500, 50});

    da_append(&wires, ((Wire) {
        .input = &nand->out,
        .out = &led->pin,
    }));

    // led->pin.wire = &wires.items[0];

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        draw_wires();
        switch_update(sw1);
        switch_update(sw2);
        led_update(led);
        nand_update(nand);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
