#ifndef FILL_H
#define FILL_H

#include "animation.h"

namespace Fill {

constexpr int kMinSpeed = 2;
constexpr int kMaxSpeed = 8;

class FillAnimation : public Animation {
  public:
    FillAnimation(CRGB* leds, int num_leds, int num_frames) :
                  Animation(leds, num_leds, num_frames) {
      fillRandomContrastingColors(c1_, c2_);
      int direction = randomDirection();
      phase_shift_ = (direction == -1) ? 0 : num_leds;
    };

    void generateNextFrame() {
      // Using modulo division determine how many are "filled"
      int fill_level = (frame_ + phase_shift_) % (1 + num_leds_ * 2); 
      if (fill_level > num_leds_) {
        fill_level = num_leds_ - (fill_level - num_leds_);
      }

      // Perform the fill
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (i < fill_level) ? c1_ : c2_;
      }
    };

  private:
    CRGB c1_, c2_;
    int phase_shift_;
};

};

#endif  // FILL_H
