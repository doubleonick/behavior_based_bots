/*
  Palette - every colour the matrix uses, in one place.

  Chosen for LEDs: more saturated than you would use on a screen.
  Some colours have two names because they mean two things (the eyes are the same
  blue as the setup screens' arrow).
*/
#pragma once
#include "Canvas.h"

namespace Palette {

typedef Canvas::Color Color;

// ---- Setup screens, spikes and behaviour rows ----
const Color WHITE  = {180, 180, 200};
const Color GREEN  = {  0, 255,  60};   // + wire, pull, approach, done
const Color RED    = {255,   0,   0};   // - wire, push, avoid
const Color GREY   = { 70,  70,  80};   // off
const Color BLUE   = {  0, 150, 255};
const Color AMBER  = {255, 110,   0};   // the wheels (and backwards)
const Color MOON   = { 40,  60, 255};   // light sensor: covered
const Color SUN    = {255, 160,   0};   // light sensor: lit
const Color TEAL   = {  0, 200, 160};   // distance sensor
const Color PURPLE = {150,   0, 255};   // distance behaviours
const Color YELLOW = {255, 200,   0};   // light behaviours

// ---- Static (into setup, and tuning back into the face) ----
const Color STATIC = {200, 200, 220};   // white, a touch blue like the setup screens

// ---- Face ----
const Color EYE       = BLUE;
const Color MOUTH     = AMBER;
const Color BROW      = {255,  30,  10};   // angry eyebrows
const Color ANGRY_EYE = {255,   0,   0};
const Color HEART     = {255,   0,  60};
const Color SLEEP_EYE = MOON;              // closed eyes when asleep

}
