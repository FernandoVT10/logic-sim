#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "raylib.h"
#include "raymath.h"

#define BG_COLOR BLACK

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

typedef struct {
    Vector2 pos;
    bool in[2];
    bool out;
    bool holded;
} AndGate;

void draw_and_gate(AndGate gate) {
    int width = 100;
    int height = 40;
    DrawRectangle(gate.pos.x, gate.pos.y, width, height, RED);

    const char *text = "AND";
    int font_size = 16;

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

typedef struct {
    Vector2 *items;
    size_t count;
    size_t capacity;
} Points;

typedef struct {
    bool on;
} SocketIn;

typedef struct {
    Points points;
    bool on;
    SocketIn *in;
} Wire;

typedef struct {
    Wire wire;
    bool on;
} SocketOut;

typedef struct {
    Vector2 pos;
    SocketOut socket;
} Switch;

void set_socket_status(SocketOut *socket, bool status) {
    socket->on = status;
    socket->wire.on = status;

    if(socket->wire.in != NULL) {
        socket->wire.in->on = status;
    }
}

void handle_switch(Switch *sw) {
    int radius = 15;
    Color switch_color;

    if(sw->socket.on) {
        switch_color = BLUE;
    } else {
        switch_color = GRAY;
    }

    DrawCircle(sw->pos.x, sw->pos.y, radius, switch_color);

    Vector2 mouse_pos = GetMousePosition();
    bool collision = CheckCollisionPointCircle(mouse_pos, sw->pos, radius);

    if(collision && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        sw->socket.on = !sw->socket.on;
        sw->socket.wire.on = sw->socket.on;
    }

    Wire wire = sw->socket.wire;
    if(wire.points.count > 1) {
        Color wire_color;

        if(wire.on) {
            wire_color = BLUE;
        } else {
            wire_color = GRAY;
        }

        for(size_t i = 0; i < wire.points.count - 1; i++) {
            Vector2 pA = wire.points.items[i];
            Vector2 pB = wire.points.items[i + 1];
            DrawLineEx(pA, pB, 5, wire_color);
        }
    }

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

typedef struct {
    Vector2 pos;
    SocketIn socket;
} BinaryDisplay;

int main() {
    InitWindow(1280, 720, "Logic Sim");

    AndGate gate = {
        .pos = {100, 100},
    };

    BinaryDisplay display = {
        .pos = {500, 15},
    };

    Switch sw = {
        .pos = {15, 15},
        .socket = {
            .wire = {
                .in = &display.socket,
            },
        },
    };

    da_append(&sw.socket.wire.points, ((Vector2){0, 15}));
    da_append(&sw.socket.wire.points, ((Vector2){500, 15}));

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);

        draw_and_gate(gate);
        update_and_gate(&gate);
        handle_switch(&sw);

        Color color;
        if(display.socket.on) {
            color = YELLOW;
        } else {
            color = GRAY;
        }

        DrawRectangle(display.pos.x, display.pos.y, 100, 100, color);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
