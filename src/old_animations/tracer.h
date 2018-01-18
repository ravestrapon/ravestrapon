#ifndef TRACER_H
#define TRACER_H

#include <FastLED.h>
#include "util.h"

namespace Tracer {

constexpr int kMinPasses = 4;
constexpr int kMaxPasses = 6;

constexpr int kMinTracerWidth = 3;
constexpr int kMaxTracerWidth = 7;

constexpr int kMinSpeed = 1;
constexpr int kMaxSpeed = 3;

constexpr int kFrameDelayMS = 12;

void TracerAnimation_Aux(CRGB *leds, int num_leds, CRGB c1, CRGB c2,
                         int tracer_width, int speed) {
  int i, j;
  
  for (i = 0; i < num_leds; i += speed) {
    for (j = 0; j < num_leds; j++) {
      leds[j] = (abs(i - j) < tracer_width) ? c1 : c2;
    }
    FastLED.show();
    FastLED.delay(kFrameDelayMS);
  }
  
  for (i = num_leds - 1; i >= 0; i -= speed) {
    for (j = 0; j < num_leds; j++) {
      leds[j] = (abs(i - j) < tracer_width) ? c1 : c2;
    }
    FastLED.show();
    FastLED.delay(kFrameDelayMS);
  }
}


void TracerAnimation(CRGB *leds, int num_leds) {
  CRGB c1, c2;
  fillRandomContrastingColors(c1, c2);
  int num_passes = random(kMinPasses, kMaxPasses);
  int tracer_width = random(kMinTracerWidth, kMaxTracerWidth);
  int speed = random(kMinSpeed, kMaxSpeed);

  for (int i = 0; i < num_passes; i++) {
    TracerAnimation_Aux(leds, num_leds, c1, c2, tracer_width, speed);
  }
}

};

#endif // TRACER_H
