#ifndef DROP_H
#define DROP_H

#include <FastLED.h>
#include "util.h"

namespace Drop {

constexpr int kMinSegmentSize = 2;
constexpr int kMaxSegmentSize = 6;

constexpr int kMinSpeed = 3;
constexpr int kMaxSpeed = 6;

constexpr int kMinFills = 2;
constexpr int kMaxFills = 4;

constexpr int kFrameDelayMS = 60;

void DropAux(CRGB *leds, int num_pole_leds, int num_ball_leds,
             int num_dropped, CRGB pole_c1, CRGB pole_c2, CRGB ball_c,
             int speed, int segment_size) {
  int fill_level = num_dropped * segment_size;
  for (int pos = 0; pos < num_pole_leds - fill_level; pos += speed) {
    for (int j = 0; j < num_pole_leds; j++) {
      leds[j] = (j >= num_pole_leds - fill_level) ? pole_c1: pole_c2;
    }

    for (int j = 0; j < segment_size && j < pos; j++) {
      leds[pos - j] = pole_c1;
    }

    FastLED.show();
    FastLED.delay(kFrameDelayMS);
  }
}

void Drop(CRGB *leds, int num_pole_leds, int num_ball_leds,
          CRGB pole_c1, CRGB pole_c2,
          CRGB ball_c1, CRGB ball_c2,
          int segment_size, int speed) {
  int num_dropped = 0;
  while (num_dropped * segment_size < num_pole_leds) {
    DropAux(leds, num_pole_leds, num_ball_leds,
            num_dropped, pole_c1, pole_c2,
            (num_dropped % 2 == 0 ) ? ball_c1 : ball_c2,
            speed, segment_size);
    num_dropped++;
  }
}

void DropAnimation(CRGB *leds, int num_pole_leds, int num_ball_leds) {
  CRGB pole_c1, pole_c2;
  fillRandomContrastingColors(pole_c1, pole_c2);
  CRGB ball_c1, ball_c2;
  fillRandomContrastingColors(ball_c1, ball_c2);
  int segment_size = random(kMinSegmentSize, kMaxSegmentSize);
  int speed = random(kMinSpeed, kMaxSpeed);
  int num_fills = random(kMinFills, kMaxFills);

  for (int i = 0; i < num_fills; i++) {
    Drop(leds, num_pole_leds, num_ball_leds,
         pole_c1, pole_c2, ball_c1, ball_c2, segment_size, speed);
    Drop(leds, num_pole_leds, num_ball_leds,
         pole_c2, pole_c1, ball_c2, ball_c1, segment_size, speed);
  }
}

};

#endif // DROP_H
