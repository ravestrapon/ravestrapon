#ifndef ANIMATION_H
#define ANIMATION_H

#include <FastLED.h>

class Animation {
  public:
    Animation(CRGB* leds, int num_leds) : leds_(leds), num_leds_(num_leds) {};
    virtual void nextFrame() = 0;

  protected:

    CRGB randomColor() const {
      return CHSV(random(255), 255, 255);
    }

    void fillRandomContrastingColors(CRGB &c1, CRGB &c2) const {
      int h1 = random(255);
      int offset = random(180) - 90;
      int h2 = (h1 + 128 + offset) % 255;
      
      c1 = CHSV(h1, 255, 255);
      c2 = CHSV(h2, 255, 255);
    }

    int randomDirection() const {
      return random(0, 2) ? 1 : -1;
    }

    CRGB* leds_;
    int num_leds_;
};

#endif  // ANIMATION_H
