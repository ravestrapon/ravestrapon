#ifndef CENTERFILL_H
#define CENTERFILL_H

#include "animation.h"

namespace CenterFill {

constexpr int kMinSpeed = 1;
constexpr int kMaxSpeed = 4;

class CenterFillAnimation : public Animation {
  public:
    CenterFillAnimation(CRGB* leds, int num_leds, int num_frames) :
              Animation(leds, num_leds, num_frames) {
      fillRandomContrastingColors(c1_, c2_);
      int direction = randomDirection();
      phase_shift_ = (direction == -1) ? 0 : num_leds / 2;
    };

    void generateNextFrame() {
      // Determine how many are "filled" out from the center
      int fill_level = (frame_ + phase_shift_) % (num_leds_ + 1); 
      if (fill_level > num_leds_ / 2) {
        fill_level = (num_leds_ / 2) - (fill_level - (num_leds_ / 2));
      }

      // Perform the fill
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (abs(num_leds_ / 2 - i) < fill_level) ? c1_ : c2_;
      }
    };

  private:
    CRGB c1_, c2_;
    int phase_shift_;
};

};

#endif  // FILL_H
