#ifndef TRACER_H
#define TRACER_H

#include "animation.h"

namespace Tracer {

constexpr int kMinTracerWidth = 3;
constexpr int kMaxTracerWidth = 15;

constexpr int kMinSpeed = 1;
constexpr int kMaxSpeed = 4;

class TracerAnimation : public Animation {
  public:
    TracerAnimation(CRGB* leds, int num_leds, int num_frames) :
          Animation(leds, num_leds, num_frames) {
      fillRandomContrastingColors(c1_, c2_);
      tracer_position_ = random(num_leds_);
      tracer_width_ = random(kMinTracerWidth, kMaxTracerWidth);
      speed_ = random(kMinSpeed, kMaxSpeed) * randomDirection();
    }

    void generateNextFrame() {
      // Draw the current frame.
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (abs(tracer_position_ - i) < tracer_width_) ? c1_ : c2_;
      }

      // Update the position of the tracer for the next frame.
      tracer_position_ += speed_;
      if (tracer_position_ <= 0) {
        tracer_position_ = 0;
        speed_ *= -1;
      } else if (tracer_position_ >= num_leds_ - 1) {
        tracer_position_ = num_leds_ - 1;
        speed_ *= -1;
      }
    }

  private:
    CRGB c1_, c2_;
    int tracer_position_, tracer_width_, speed_;
};

};

#endif // TRACER_H
