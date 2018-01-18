#ifndef STRIPES_H
#define STRIPES_H

#include "animation.h"

namespace Stripes {

constexpr int kMinNumStripes = 2;
constexpr int kMaxNumStripes = 5;

constexpr int kMinSweeps = 5;
constexpr int kMaxSweeps = 10;

constexpr int kMinSpeed = 2;
constexpr int kMaxSpeed = 4;

class StripesAnimation : public Animation {
  public:
    StripesAnimation(CRGB* leds, int num_leds) : Animation(leds, num_leds) {
      fillRandomContrastingColors(c1_, c2_);
      int num_stripes = random(kMinNumStripes, kMaxNumStripes);
      stripe_size_ = num_leds_ / num_stripes;
      speed_ = random(kMinSpeed, kMaxSpeed);
      direction_ = randomDirection();

      offset_ = 0;
    };

    void nextFrame() {
      offset_ += direction_ * speed_;
      for(int i = 0; i < num_leds_; i++) {
        leds_[i] = (((i + offset_) / stripe_size_) % 2 == 0) ? c1_ : c2_;
      }
    };

  private:
    CRGB c1_, c2_;
    int stripe_size_;
    int direction_;
    int speed_;

    int offset_; 
};


};

#endif //STRIPES_H
