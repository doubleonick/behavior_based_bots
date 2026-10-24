/*
  Icons - the 5x5 pictures the setup screens use, in one place.

  Each picture is 5 rows, top to bottom; '#' = lit, '.' = off.
*/
#pragma once
#include <Arduino.h>

namespace Icons {

const char* const PLUS[5]  = {".....", ".....", "..#..", ".###.", "..#.."};
const char* const MINUS[5] = {".....", ".....", ".....", ".###.", "....."};
const char* const CROSS[5] = {".....", ".....", ".#.#.", "..#..", ".#.#."};
const char* const MOON[5]  = {".....", ".##..", "#....", "#....", ".##.."};   // light: covered
const char* const SUN[5]   = {".....", "#..#.", ".##..", ".##..", "#..#."};   // light: lit
const char* const FAR[5]   = {".....", ".....", ".##..", ".##..", "....."};   // small = far away
const char* const NEAR[5]  = {".....", "####.", "#..#.", "#..#.", "####."};   // big = close
const char* const CHECK[5] = {".....", "....#", "...#.", "#.#..", ".#..."};

// The "forward" arrow, as (row, col) pairs from the tip down
const int8_t ARROW[9][2] = {{0, 2}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 2}, {2, 4}, {3, 2}, {4, 2}};

// Where the sensor dots go on the top row, for 1, 2 or 3 sensors
const int8_t DOT_COLS[4][3] = {{0, 0, 0}, {2, 0, 0}, {1, 3, 0}, {0, 2, 4}};

}
