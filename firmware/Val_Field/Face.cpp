#include "Face.h"
#include "Static.h"
#include "Palette.h"
#include <math.h>

// =====================================================================
//  Tunable look
// =====================================================================
namespace {

typedef Canvas::Color Color;

// Colours: see Palette.h
using Palette::EYE;
using Palette::MOUTH;
using Palette::BROW;
using Palette::ANGRY_EYE;
using Palette::HEART;
using Palette::SLEEP_EYE;

const float BASE = 0.6f;            // normal intensity; 1.0 is kept for "max intensity" moments
const uint32_t TUNE_IN_MS = 600;    // static settling into the face
const float TAU = 6.2831853f;

// ---------------------------------------------------------------------
//  Face layouts. Each string is one row, top to bottom.
//  E = eye, M = mouth, B = brow, H = heart, Z = sleeping eye, . = off
// ---------------------------------------------------------------------
const char* const ROWS_NEUTRAL[5]    = {".....", ".E.E.", ".....", ".MMM.", "....."};
const char* const ROWS_HAPPY[5]      = {".....", ".E.E.", ".....", "M...M", ".MMM."};
const char* const ROWS_LAUGHING[5]   = {".E.E.", "E.E.E", ".....", "MMMMM", ".MMM."};
const char* const ROWS_WINK[5]       = {".....", ".E.EE", ".....", "....M", ".MMM."};
const char* const ROWS_SURPRISED_EYES[5] = {".E.E.", ".....", ".....", ".....", "....."};
const char* const ROWS_ANGRY_BASE[5] = {"B...B", ".....", ".....", ".....", "M...M"};
const char* const ROWS_HEART[5]      = {".H.H.", "HHHHH", "HHHHH", ".HHH.", "..H.."};

// Mouth-only layouts (eyes are drawn separately so they can blink and drift)
const char* const MOUTH_NEUTRAL[5]   = {".....", ".....", ".....", ".MMM.", "....."};
const char* const MOUTH_HAPPY[5]     = {".....", ".....", ".....", "M...M", ".MMM."};
const char* const MOUTH_SAD[5]       = {".....", ".....", ".....", ".MMM.", "M...M"};
const char* const MOUTH_UNSURE[5]    = {".....", ".....", ".....", ".MM..", "...M."};

// Talking shapes. Eyes sit on the second row unless the mouth reaches the third row.
const char* const TALK_ROWS[6][5] = {
  {".....", ".E.E.", ".....", ".MMM.", "....."},   // MOUTH_CLOSED
  {".....", ".E.E.", ".....", ".MMM.", ".MMM."},   // MOUTH_HALF
  {".E.E.", ".....", ".MMM.", ".M.M.", ".MMM."},   // MOUTH_WIDE
  {".....", ".E.E.", ".....", "MMMMM", "....."},   // MOUTH_STRETCHED
  {".E.E.", ".....", "..M..", ".M.M.", "..M.."},   // MOUTH_ROUND
  {".....", ".E.E.", ".....", "..M..", "....."},   // MOUTH_SMALL
};

// "Hello, my name is, Val"
const Face::Mouth HELLO_VAL[] = {
  // Hello
  Face::MOUTH_HALF, Face::MOUTH_STRETCHED, Face::MOUTH_HALF, Face::MOUTH_ROUND, Face::MOUTH_SMALL,
  Face::MOUTH_CLOSED, Face::MOUTH_CLOSED,
  // my
  Face::MOUTH_CLOSED, Face::MOUTH_WIDE, Face::MOUTH_STRETCHED,
  // name
  Face::MOUTH_HALF, Face::MOUTH_STRETCHED, Face::MOUTH_STRETCHED, Face::MOUTH_CLOSED,
  // is
  Face::MOUTH_STRETCHED, Face::MOUTH_HALF,
  Face::MOUTH_CLOSED,
  // Val
  Face::MOUTH_HALF, Face::MOUTH_WIDE, Face::MOUTH_WIDE, Face::MOUTH_HALF,
  Face::MOUTH_CLOSED, Face::MOUTH_CLOSED,
};

// =====================================================================
//  Drawing helpers (pixels, soft pixels and blends are Canvas's)
// =====================================================================
const Color* colorFor(char ch) {
  switch (ch) {
    case 'E': return &EYE;
    case 'M': return &MOUTH;
    case 'B': return &BROW;
    case 'H': return &HEART;
    case 'Z': return &SLEEP_EYE;
    default:  return nullptr;
  }
}

// Draw a 5-row face layout (adds to whatever is already on the canvas)
void addRows(Canvas& g, const char* const rows[5], float i = BASE) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 5; c++) {
      const Color* col = colorFor(rows[r][c]);
      if (col) {
        g.put(r, c, *col, i);
      }
    }
  }
}

Color mix(const Color& a, const Color& b, float k) {
  return {a.r + (b.r - a.r) * k, a.g + (b.g - a.g) * k, a.b + (b.b - a.b) * k};
}

float smooth(float x) { return x <= 0 ? 0 : (x >= 1 ? 1 : x * x * (3 - 2 * x)); }

// 0 before a, 1 after b, easing smoothly in between
float ramp(float x, float a, float b) { return smooth((x - a) / (b - a)); }

// Eyelids: 1 = open, dipping to 0 at each blink time (seconds)
float blink(float x, const float* times, int count) {
  float open = 1;
  for (int n = 0; n < count; n++) {
    open = fminf(open, smooth(fabsf(x - times[n]) / 0.14f));
  }
  return open;
}

// Flickering intensity for a mouth pixel in column c
float shimmer(float t, int c) {
  return 0.25f + 0.75f * (0.5f + 0.5f * sinf(t * TAU * 4 + c * 2.1f));
}

}  // namespace

// =====================================================================
//  Face
// =====================================================================
Face::Face(CRGB* leds) : _leds(leds) {}

void Face::show(Expression e) {
  _expr = e;
  _tuning = false;
  _startMs = millis();
  _timer.restart();   // draw the first frame straight away
}

void Face::tuneIn(Expression e) {
  show(e);
  _tuning = true;
}

void Face::neutral()   { show(NEUTRAL); }
void Face::happy()     { show(HAPPY); }
void Face::sad()       { show(SAD); }
void Face::surprised() { show(SURPRISED); }
void Face::angry()     { show(ANGRY); }
void Face::laughing()  { show(LAUGHING); }
void Face::wink()      { show(WINK); }
void Face::unsure()    { show(UNSURE); }
void Face::asleep()    { show(ASLEEP); }
void Face::love()      { show(LOVE); }
void Face::scared()    { show(SCARED); }
void Face::curious()   { show(CURIOUS); }

void Face::say(const Mouth* shapes, uint8_t count, uint16_t frameMs, Expression then) {
  _sayShapes = shapes;
  _sayCount = count;
  _sayFrameMs = frameMs ? frameMs : 150;
  _sayThen = then;
  show(TALKING);
}

void Face::sayHello() {
  say(HELLO_VAL, sizeof(HELLO_VAL) / sizeof(HELLO_VAL[0]), 150, NEUTRAL);
}

const char* Face::name(Expression e) {
  switch (e) {
    case NEUTRAL:   return "Neutral";
    case HAPPY:     return "Happy";
    case SAD:       return "Sad";
    case SURPRISED: return "Surprised";
    case ANGRY:     return "Angry";
    case LAUGHING:  return "Laughing";
    case WINK:      return "Wink";
    case UNSURE:    return "Unsure";
    case ASLEEP:    return "Asleep";
    case LOVE:      return "Love";
    case SCARED:    return "Scared";
    case CURIOUS:   return "Curious";
    case TALKING:   return "Talking";
  }
  return "?";
}

bool Face::update() {
  if (!_timer.due()) {
    return false;
  }
  uint32_t now = millis();

  // Finished talking? Move on to the follow-up expression.
  if (_expr == TALKING && (now - _startMs) / _sayFrameMs >= _sayCount) {
    _expr = _sayThen;
    _startMs = now;
  }

  // Tuning in: the expression's first frame, partly hidden by static.
  // When it has settled, the expression starts playing from its beginning.
  if (_tuning) {
    uint32_t elapsed = now - _startMs;
    if (elapsed >= TUNE_IN_MS) {
      _tuning = false;
      _startMs = now;
    } else {
      _canvas.clear();
      drawExpression(_expr, 0, 0);
      addStatic((float)elapsed / TUNE_IN_MS, elapsed);
      _canvas.push(_leds);
      return true;
    }
  }

  _canvas.clear();
  drawExpression(_expr, (now - _startMs) / 1000.0f, now - _startMs);
  _canvas.push(_leds);
  return true;
}

void Face::drawExpression(Expression e, float t, uint32_t elapsedMs) {
  switch (e) {
    case NEUTRAL:   drawNeutral(t);   break;
    case HAPPY:     drawHappy(t);     break;
    case SAD:       drawSad(t);       break;
    case SURPRISED: drawSurprised(t); break;
    case ANGRY:     drawAngry(t);     break;
    case LAUGHING:  drawLaughing(t);  break;
    case WINK:      drawWink(t);      break;
    case UNSURE:    drawUnsure(t);    break;
    case ASLEEP:    drawAsleep(t);    break;
    case LOVE:      drawLove(t);      break;
    case SCARED:    drawScared(t);    break;
    case CURIOUS:   drawCurious(t);   break;
    case TALKING:   drawTalking(elapsedMs); break;
  }
}

// Each LED shows static until its own settling moment, then shows the face
// (its face colour, or dark if it isn't part of the face).
void Face::addStatic(float progress, uint32_t elapsedMs) {
  uint32_t frame = 100 + elapsedMs / Static::FRAME_MS;   // a different run of static from setup's
  for (int k = 0; k < 25; k++) {
    if (progress >= Static::settleAt(k)) {
      continue;                          // settled: keep the face's pixel
    }
    _canvas.px[k].c = Palette::STATIC;
    _canvas.px[k].i = Static::level(frame, k);
  }
}

// Eyes on the second row (columns 1 and 3) that can slide sideways and blink, plus a mouth
void Face::drawEyesAndMouth(const char* const mouthRows[5], float drift, float eyelids) {
  addRows(_canvas, mouthRows);
  _canvas.putSoft(1, 1 + drift, EYE, BASE * eyelids);
  _canvas.putSoft(1, 3 + drift, EYE, BASE * eyelids);
}

// ---------------------------------------------------------------------
//  Expressions
// ---------------------------------------------------------------------

// Blink, glance left, glance right, double blink (7 second cycle)
void Face::drawNeutral(float t) {
  float x = fmodf(t, 7.0f);
  float drift = -ramp(x, 2.0f, 2.4f) + ramp(x, 3.2f, 3.6f) + ramp(x, 4.2f, 4.6f) - ramp(x, 5.4f, 5.8f);
  const float blinks[] = {1.0f, 6.3f, 6.65f};
  drawEyesAndMouth(MOUTH_NEUTRAL, drift, blink(x, blinks, 3));
}

void Face::drawHappy(float t) {
  const float blinks[] = {2.5f};
  drawEyesAndMouth(MOUTH_HAPPY, 0, blink(fmodf(t + 1.5f, 5.0f), blinks, 1));
}

void Face::drawSad(float t) {
  const float blinks[] = {2.5f, 2.9f};
  drawEyesAndMouth(MOUTH_SAD, 0, blink(fmodf(t + 3.0f, 6.0f), blinks, 2));
}

// Crooked mouth, a glance to one side, a blink (6 second cycle)
void Face::drawUnsure(float t) {
  float x = fmodf(t, 6.0f);
  float drift = -ramp(x, 1.0f, 1.4f) + ramp(x, 2.6f, 3.0f);
  const float blinks[] = {4.5f};
  drawEyesAndMouth(MOUTH_UNSURE, drift, blink(x, blinks, 1));
}

// Happy -> Laughing, with the three upper-middle mouth pixels shimmering
void Face::drawLaughing(float t) {
  float x = fmodf(t, 5.5f);
  Canvas a, b;
  a.clear(); addRows(a, ROWS_HAPPY);
  b.clear(); addRows(b, ROWS_LAUGHING);
  for (int c = 1; c <= 3; c++) {
    b.px[3 * 5 + c].i = shimmer(t, c);
  }
  _canvas.blend(a, b, ramp(x, 1.2f, 1.6f) - ramp(x, 4.2f, 4.6f));
}

// Neutral -> "O" mouth. One bright pixel laps the O, then every mouth pixel
// goes to max intensity, holds for a beat, and the face returns to neutral.
void Face::drawSurprised(float t) {
  float x = fmodf(t, 6.0f);
  static const int8_t ring[8][2] = {{2, 1}, {2, 2}, {2, 3}, {3, 3}, {4, 3}, {4, 2}, {4, 1}, {3, 1}};
  const float start = 1.4f, step = 0.13f;
  const float lapEnd = start + 9 * step, holdEnd = lapEnd + 0.75f;
  int pos = (int)floorf((x - start) / step);

  Canvas a, b;
  a.clear(); addRows(a, ROWS_NEUTRAL);
  b.clear(); addRows(b, ROWS_SURPRISED_EYES);
  for (int j = 0; j < 8; j++) {
    float i = 0.4f;
    if (x >= lapEnd) {
      i = 1;
    } else if (x >= start) {
      if (pos % 8 == j) {
        i = 1;                                // the travelling pixel
      } else if (pos > 0 && (pos + 7) % 8 == j) {
        i = 0.65f;                            // short trail behind it
      }
    }
    b.put(ring[j][0], ring[j][1], MOUTH, i);
  }
  _canvas.blend(a, b, ramp(x, 0.8f, 1.1f) - ramp(x, holdEnd, holdEnd + 0.4f));
}

// Eyes go from blue to red and brighten; the mouth shimmer builds with the anger
void Face::drawAngry(float t) {
  float x = fmodf(t, 6.0f);
  float k = ramp(x, 0.8f, 2.4f) - ramp(x, 5.2f, 5.8f);
  addRows(_canvas, ROWS_ANGRY_BASE);
  Color eye = mix(EYE, ANGRY_EYE, k);
  _canvas.put(1, 1, eye, BASE + 0.4f * k);
  _canvas.put(1, 3, eye, BASE + 0.4f * k);
  for (int c = 1; c <= 3; c++) {
    _canvas.put(3, c, MOUTH, BASE + k * (shimmer(t, c) - BASE));
  }
}

// Neutral, then one eye closes and the mouth pulls into a one-sided smirk
void Face::drawWink(float t) {
  float x = fmodf(t, 5.0f);
  Canvas a, b;
  a.clear(); addRows(a, ROWS_NEUTRAL);
  b.clear(); addRows(b, ROWS_WINK);
  _canvas.blend(a, b, ramp(x, 1.2f, 1.5f) - ramp(x, 3.6f, 3.9f));
}

// The whole face rises and falls one row either side of centre, keeping the
// eye-to-mouth spacing. Near the top the small mouth opens into the round
// shape and closes again: a snore.
void Face::drawAsleep(float t) {
  float y = sinf(t * TAU / 6.0f);          // -1 = top, +1 = bottom, 6 second cycle
  _canvas.putSoft(1 + y, 0, SLEEP_EYE, BASE);
  _canvas.putSoft(1 + y, 1, SLEEP_EYE, BASE);
  _canvas.putSoft(1 + y, 3, SLEEP_EYE, BASE);
  _canvas.putSoft(1 + y, 4, SLEEP_EYE, BASE);
  _canvas.putSoft(3 + y, 2, MOUTH, BASE);
  float snore = smooth((-y - 0.6f) / 0.35f);
  _canvas.putSoft(4 + y, 1, MOUTH, BASE * snore);
  _canvas.putSoft(4 + y, 3, MOUTH, BASE * snore);
  _canvas.putSoft(5 + y, 2, MOUTH, BASE * snore);
}

// Brightness spreads out from the centre of the heart and draws back in
void Face::drawLove(float t) {
  float reach = 3.2f * (0.5f - 0.5f * cosf(t * TAU / 1.8f)) - 0.3f;
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 5; c++) {
      if (ROWS_HEART[r][c] != 'H') {
        continue;
      }
      float d = sqrtf((r - 1.8f) * (r - 1.8f) + (c - 2.0f) * (c - 2.0f));
      float lit = reach - d + 0.6f;
      lit = lit < 0 ? 0 : (lit > 1 ? 1 : lit);
      _canvas.put(r, c, HEART, 0.3f + 0.7f * lit);
    }
  }
}

// Tall eyes (two pixels each) that tremble from side to side, and a zigzag mouth
// that wobbles, flipping between its two shapes
void Face::drawScared(float t) {
  float shake = 0.25f * sinf(t * TAU * 5);           // small, fast tremble
  for (int r = 0; r <= 1; r++) {
    _canvas.putSoft(r, 1 + shake, EYE, BASE);
    _canvas.putSoft(r, 3 + shake, EYE, BASE);
  }
  bool flip = ((int)(t / 0.3f)) % 2 == 1;
  for (int c = 0; c < 5; c++) {
    bool high = (c % 2 == 0);                         // columns 0, 2, 4 on the upper row
    if (flip) {
      high = !high;
    }
    _canvas.put(high ? 3 : 4, c, MOUTH, BASE);
  }
}

// Eyes slide up and off to the right, stay there a while (spotting something),
// blink, and come back; a smile the whole time (6 second cycle)
void Face::drawCurious(float t) {
  float x = fmodf(t, 6.0f);
  float k = ramp(x, 0.6f, 1.2f) - ramp(x, 4.6f, 5.2f);  // 0 = looking ahead, 1 = looking off
  const float blinks[] = {3.4f};
  float eyelids = blink(x, blinks, 1);
  addRows(_canvas, MOUTH_HAPPY);
  _canvas.putSoft(1 - k, 1 + k, EYE, BASE * eyelids);
  _canvas.putSoft(1 - k, 3 + k, EYE, BASE * eyelids);
}

// One mouth shape per frame from the sequence given to say()
void Face::drawTalking(uint32_t elapsedMs) {
  if (!_sayShapes || _sayCount == 0) {
    addRows(_canvas, TALK_ROWS[MOUTH_CLOSED]);
    return;
  }
  uint32_t n = elapsedMs / _sayFrameMs;
  if (n >= _sayCount) {
    n = _sayCount - 1;
  }
  addRows(_canvas, TALK_ROWS[_sayShapes[n]]);
}
