#ifndef STRIPES_H
#define STRIPES_H

#include <FastLED.h>
#include "util.h"

namespace Stripes {

constexpr int kMinNumStripes = 2;
constexpr int kMaxNumStripes = 5;

constexpr int kMinSweeps = 5;
constexpr int kMaxSweeps = 10;

constexpr int kMinSpeed = 2;
constexpr int kMaxSpeed = 4;

constexpr int kFrameDelayMS = 50;

void StripesAnimation(CRGB *leds, int num_leds) {
  CRGB c1, c2;
  fillRandomContrastingColors(c1, c2);

  int num_stripes = random(kMinNumStripes, kMaxNumStripes);
  int stripe_size = num_leds / num_stripes;
  int num_sweeps = random(kMinSweeps, kMaxSweeps);
  int dir = random(0, 2) ? 1 : -1;
  int speed = random(kMinSpeed, kMaxSpeed);

  for (int offset = 0;
       offset < num_leds * num_sweeps && offset > -num_leds * num_sweeps;
       offset += dir * speed) {
    for(int j = 0; j < num_leds; j++) {
      leds[j] = (((j + offset) / stripe_size) % 2 == 0) ? c1 : c2;
    }
    FastLED.show();
    FastLED.delay(kFrameDelayMS);
  }
}

};

#endif //STRIPES_H
