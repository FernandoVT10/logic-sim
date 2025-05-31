#!/bin/bash
FLAGS="-Wall -Wextra -Werror"
RAYLIB="-I./raylib/include -L./raylib/lib -l:libraylib.a -lm"
FILES="src/main.c"
gcc $FLAGS -o main $FILES $RAYLIB
