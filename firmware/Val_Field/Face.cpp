#include "Face.h"
#include "Static.h"
#include <math.h>

// =====================================================================
//  Tunable look
// =====================================================================
namespace {

typedef Face::Color Color;
typedef Face::Pixel Pixel;

// Colours, chosen for LEDs (more saturated than you would use on a screen)
const Color EYE       = {  0, 150, 255};   // blue
const Color MOUTH     = {255, 110,   0};   // amber
const Color BROW      = {255,  30,  10};   // angry eyebrows
const Color ANGRY_EYE = {255,   0,   0};
const Color HEART     = {255,   0,  60};
const Color SLEEP_EYE = { 40,  60, 255};   // closed eyes when asleep

const float BASE = 0.6f;            // normal intensity; 1.0 is kept for "max intensity" moments
const uint32_t FRAME_MS = 40;       // redraw period (25 frames per second)
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
//  Drawing helpers. A frame is 25 pixels, index = row * 5 + col.
// =====================================================================
void clearFrame(Pixel* g) {
  for (int n = 0; n < 25; n++) {
    g[n].c = {0, 0, 0};
    g[n].i = 0;
  }
}

// Light one pixel. If two things land on the same pixel, the brighter one wins.
void put(Pixel* g, int row, int col, const Color& c, float i) {
  if (row < 0 || row > 4 || col < 0 || col > 4 || i <= 0) {
    return;
  }
  Pixel& p = g[row * 5 + col];
  if (i > p.i) {
    p.i = i > 1 ? 1 : i;
    p.c = c;
  }
}

// Light a "pixel" at a fractional position by sharing its brightness between
// the neighbouring LEDs. This is what makes eyes slide instead of jump.
void putSoft(Pixel* g, float rowF, float colF, const Color& c, float i) {
  int r0 = (int)floorf(rowF), c0 = (int)floorf(colF);
  for (int r = r0; r <= r0 + 1; r++) {
    for (int col = c0; col <= c0 + 1; col++) {
      float w = fmaxf(0, 1 - fabsf(r - rowF)) * fmaxf(0, 1 - fabsf(col - colF));
      put(g, r, col, c, i * w);
    }
  }
}

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

// Fill a frame from a 5-row layout (adds to whatever is already in the frame)
void addRows(Pixel* g, const char* const rows[5], float i = BASE) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 5; c++) {
      const Color* col = colorFor(rows[r][c]);
      if (col) {
        put(g, r, c, *col, i);
      }
    }
  }
}

// Cross-fade: k = 0 gives frame a, k = 1 gives frame b
void blend(Pixel* out, const Pixel* a, const Pixel* b, float k) {
  for (int n = 0; n < 25; n++) {
    float ia = a[n].i * (1 - k), ib = b[n].i * k;
    out[n].c = (ib > ia) ? b[n].c : a[n].c;
    out[n].i = ia + ib;
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
Face::Face(CRGB* leds) : _leds(leds) { clearFrame(_frame); }

void Face::show(Expression e) {
  _expr = e;
  _tuning = false;
  _startMs = millis();
  _lastDrawMs = 0;   // draw the first frame straight away
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
    case TALKING:   return "Talking";
  }
  return "?";
}

bool Face::update() {
  uint32_t now = millis();
  if (_lastDrawMs != 0 && now - _lastDrawMs < FRAME_MS) {
    return false;
  }
  _lastDrawMs = now ? now : 1;

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
      clearFrame(_frame);
      drawExpression(_expr, 0, 0);
      addStatic((float)elapsed / TUNE_IN_MS, elapsed);
      push();
      return true;
    }
  }

  clearFrame(_frame);
  drawExpression(_expr, (now - _startMs) / 1000.0f, now - _startMs);
  push();
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
    case TALKING:   drawTalking(elapsedMs); break;
  }
}

// Each LED shows static until its own settling moment, then shows the face
// (its face colour, or dark if it isn't part of the face).
void Face::addStatic(float progress, uint32_t elapsedMs) {
  uint32_t frame = 100 + elapsedMs / Static::FRAME_MS;   // a different run of static from setup's
  const Color white = {(float)Static::COLOR[0], (float)Static::COLOR[1], (float)Static::COLOR[2]};
  for (int k = 0; k < 25; k++) {
    if (progress >= Static::settleAt(k)) {
      continue;                          // settled: keep the face's pixel
    }
    _frame[k].c = white;
    _frame[k].i = Static::level(frame, k);
  }
}

// Copy the frame to the LEDs. Intensity is squared so that the difference
// between normal (0.6) and max (1.0) is clearly visible on real LEDs.
void Face::push() {
  for (int n = 0; n < 25; n++) {
    const Pixel& p = _frame[n];
    if (p.i <= 0.02f) {
      _leds[n] = CRGB::Black;
      continue;
    }
    float s = p.i * p.i;
    _leds[n] = CRGB((uint8_t)(p.c.r * s), (uint8_t)(p.c.g * s), (uint8_t)(p.c.b * s));
  }
  FastLED.show();
}

// Eyes on the second row (columns 1 and 3) that can slide sideways and blink, plus a mouth
void Face::drawEyesAndMouth(const char* const mouthRows[5], float drift, float eyelids) {
  addRows(_frame, mouthRows);
  putSoft(_frame, 1, 1 + drift, EYE, BASE * eyelids);
  putSoft(_frame, 1, 3 + drift, EYE, BASE * eyelids);
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
  Pixel a[25], b[25];
  clearFrame(a); addRows(a, ROWS_HAPPY);
  clearFrame(b); addRows(b, ROWS_LAUGHING);
  for (int c = 1; c <= 3; c++) {
    b[3 * 5 + c].i = shimmer(t, c);
  }
  blend(_frame, a, b, ramp(x, 1.2f, 1.6f) - ramp(x, 4.2f, 4.6f));
}

// Neutral -> "O" mouth. One bright pixel laps the O, then every mouth pixel
// goes to max intensity, holds for a beat, and the face returns to neutral.
void Face::drawSurprised(float t) {
  float x = fmodf(t, 6.0f);
  static const int8_t ring[8][2] = {{2, 1}, {2, 2}, {2, 3}, {3, 3}, {4, 3}, {4, 2}, {4, 1}, {3, 1}};
  const float start = 1.4f, step = 0.13f;
  const float lapEnd = start + 9 * step, holdEnd = lapEnd + 0.75f;
  int pos = (int)floorf((x - start) / step);

  Pixel a[25], b[25];
  clearFrame(a); addRows(a, ROWS_NEUTRAL);
  clearFrame(b); addRows(b, ROWS_SURPRISED_EYES);
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
    put(b, ring[j][0], ring[j][1], MOUTH, i);
  }
  blend(_frame, a, b, ramp(x, 0.8f, 1.1f) - ramp(x, holdEnd, holdEnd + 0.4f));
}

// Eyes go from blue to red and brighten; the mouth shimmer builds with the anger
void Face::drawAngry(float t) {
  float x = fmodf(t, 6.0f);
  float k = ramp(x, 0.8f, 2.4f) - ramp(x, 5.2f, 5.8f);
  addRows(_frame, ROWS_ANGRY_BASE);
  Color eye = mix(EYE, ANGRY_EYE, k);
  put(_frame, 1, 1, eye, BASE + 0.4f * k);
  put(_frame, 1, 3, eye, BASE + 0.4f * k);
  for (int c = 1; c <= 3; c++) {
    put(_frame, 3, c, MOUTH, BASE + k * (shimmer(t, c) - BASE));
  }
}

// Neutral, then one eye closes and the mouth pulls into a one-sided smirk
void Face::drawWink(float t) {
  float x = fmodf(t, 5.0f);
  Pixel a[25], b[25];
  clearFrame(a); addRows(a, ROWS_NEUTRAL);
  clearFrame(b); addRows(b, ROWS_WINK);
  blend(_frame, a, b, ramp(x, 1.2f, 1.5f) - ramp(x, 3.6f, 3.9f));
}

// The whole face rises and falls one row either side of centre, keeping the
// eye-to-mouth spacing. Near the top the small mouth opens into the round
// shape and closes again: a snore.
void Face::drawAsleep(float t) {
  float y = sinf(t * TAU / 6.0f);          // -1 = top, +1 = bottom, 6 second cycle
  putSoft(_frame, 1 + y, 0, SLEEP_EYE, BASE);
  putSoft(_frame, 1 + y, 1, SLEEP_EYE, BASE);
  putSoft(_frame, 1 + y, 3, SLEEP_EYE, BASE);
  putSoft(_frame, 1 + y, 4, SLEEP_EYE, BASE);
  putSoft(_frame, 3 + y, 2, MOUTH, BASE);
  float snore = smooth((-y - 0.6f) / 0.35f);
  putSoft(_frame, 4 + y, 1, MOUTH, BASE * snore);
  putSoft(_frame, 4 + y, 3, MOUTH, BASE * snore);
  putSoft(_frame, 5 + y, 2, MOUTH, BASE * snore);
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
      put(_frame, r, c, HEART, 0.3f + 0.7f * lit);
    }
  }
}

// One mouth shape per frame from the sequence given to say()
void Face::drawTalking(uint32_t elapsedMs) {
  if (!_sayShapes || _sayCount == 0) {
    addRows(_frame, TALK_ROWS[MOUTH_CLOSED]);
    return;
  }
  uint32_t n = elapsedMs / _sayFrameMs;
  if (n >= _sayCount) {
    n = _sayCount - 1;
  }
  addRows(_frame, TALK_ROWS[_sayShapes[n]]);
}
