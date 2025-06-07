#ifndef ACTIONS_H
#define ACTIONS_H

#include "raylib.h"
#include "components.h"

void drag_start(Vector2 *pos);
void drag_update();

void wiring_start(Pin *pin);
void wiring_end(Pin *pin);
void wiring_update();

#endif // ACTIONS_H
