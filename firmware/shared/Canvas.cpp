#include "Canvas.h"
#include <math.h>

void Canvas::clear() {
  for (int n = 0; n < 25; n++) {
    px[n].c = {0, 0, 0};
    px[n].i = 0;
  }
}

void Canvas::put(int row, int col, const Color& c, float intensity) {
  if (row < 0 || row > 4 || col < 0 || col > 4 || intensity <= 0) {
    return;
  }
  Pixel& p = px[row * 5 + col];
  if (intensity > p.i) {
    p.i = intensity > 1 ? 1 : intensity;
    p.c = c;
  }
}

void Canvas::putSoft(float row, float col, const Color& c, float intensity) {
  int r0 = (int)floorf(row);
  int c0 = (int)floorf(col);
  for (int r = r0; r <= r0 + 1; r++) {
    for (int k = c0; k <= c0 + 1; k++) {
      float w = fmaxf(0, 1 - fabsf(r - row)) * fmaxf(0, 1 - fabsf(k - col));
      put(r, k, c, intensity * w);
    }
  }
}

void Canvas::putRows(const char* const rows[5], char mark, const Color& c, float intensity) {
  for (int r = 0; r < 5; r++) {
    for (int k = 0; k < 5; k++) {
      if (rows[r][k] == mark) {
        put(r, k, c, intensity);
      }
    }
  }
}

void Canvas::blend(const Canvas& a, const Canvas& b, float k) {
  for (int n = 0; n < 25; n++) {
    float ia = a.px[n].i * (1 - k);
    float ib = b.px[n].i * k;
    px[n].c = (ib > ia) ? b.px[n].c : a.px[n].c;
    px[n].i = ia + ib;
  }
}

void Canvas::push(CRGB* leds) const {
  for (int n = 0; n < 25; n++) {
    const Pixel& p = px[n];
    if (p.i <= 0.02f) {
      leds[n] = CRGB::Black;
      continue;
    }
    float s = p.i * p.i;
    leds[n] = CRGB((uint8_t)(p.c.r * s), (uint8_t)(p.c.g * s), (uint8_t)(p.c.b * s));
  }
  FastLED.show();
}

bool FrameTimer::due() {
  uint32_t now = millis();
  if (_lastMs != 0 && now - _lastMs < FRAME_MS) {
    return false;
  }
  _lastMs = now ? now : 1;
  return true;
}
