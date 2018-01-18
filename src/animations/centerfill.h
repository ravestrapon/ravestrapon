#ifndef CENTERFILL_H
#define CENTERFILL_H

#include "animation.h"

namespace CenterFill {

constexpr int kMinSpeed = 1;
constexpr int kMaxSpeed = 4;

class CenterFillAnimation : public Animation {
  public:
    CenterFillAnimation(CRGB* leds, int num_leds) : Animation(leds, num_leds) {
      fillRandomContrastingColors(c1_, c2_);
      int direction = randomDirection();
      frame_ = (direction == -1) ? 0 : num_leds / 2;
    };

    void nextFrame() {
      // Determine how many are "filled" out from the center
      int fill_level = frame_ % (num_leds_ + 1); 
      if (fill_level > num_leds_ / 2) {
        fill_level = (num_leds_ / 2) - (fill_level - (num_leds_ / 2));
      }

      // Perform the fill
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (abs(num_leds_ / 2 - i) < fill_level) ? c1_ : c2_;
      }

      frame_++;
    };

  private:
    CRGB c1_, c2_;
    int frame_;
};

};

#endif  // FILL_H
