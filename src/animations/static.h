#ifndef STATIC_H
#define STATIC_H

#include <FastLED.h>
#include "util.h"

namespace Static {

constexpr int kNumFrames = 35;
constexpr int kNumPulses = 5;
constexpr int kFrameDelayMS = 75;
constexpr int kMinSegmentLength = 3;
constexpr int kMaxSegmentLength = 12;

void StaticAnimation(CRGB *leds, int num_leds) {
  int segment_length = random(kMinSegmentLength, kMaxSegmentLength);
  int perc = 0;
  int dir = 1;

  for (int i = 0; i < kNumFrames; i++) {
    CRGB c = randomColor();
    for (int j = 0; j < num_leds; j++) {
      if (j % segment_length == 0) {
        c = randomColor();
      }
      leds[j] = c;
    }


    FastLED.show();
    FastLED.delay(kFrameDelayMS);
  }
}

};

#endif // STATIC_H
