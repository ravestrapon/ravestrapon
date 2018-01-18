#ifndef FILL_H
#define FILL_H

#include "animation.h"

namespace Fill {

constexpr int kMinSpeed = 2;
constexpr int kMaxSpeed = 8;

class FillAnimation : public Animation {
  public:
    FillAnimation(CRGB* leds, int num_leds) : Animation(leds, num_leds) {
      fillRandomContrastingColors(c1_, c2_);
      int direction = randomDirection();
      frame_ = (direction == -1) ? 0 : num_leds;
    };

    void nextFrame() {
      // Using modulo division determine how many are "filled"
      int fill_level = frame_ % (1 + num_leds_ * 2); 
      if (fill_level > num_leds_) {
        fill_level = num_leds_ - (fill_level - num_leds_);
      }

      // Perform the fill
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (i < fill_level) ? c1_ : c2_;
      }

      frame_++;
    };

  private:
    CRGB c1_, c2_;
    int frame_;
};

};

#endif  // FILL_H
