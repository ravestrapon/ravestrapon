#ifndef STATIC_H
#define STATIC_H

#include "animation.h"

namespace Static {

constexpr int kMinSegments = 3;
constexpr int kMaxSegments = 6; 

// Note: For this animation the speed is the number of frames between
// color changes.  This means a higher "speed" actually results in the
// colors changing more slowly.
constexpr int kMinSpeed = 5;
constexpr int kMaxSpeed = 10;

class StaticAnimation : public Animation {
  public:
    StaticAnimation(CRGB* leds, int num_leds) : Animation(leds, num_leds) {
      int num_segments = random(kMinSegments, kMaxSegments);
      segment_length_ = num_leds_ / num_segments;
      speed_ = random(kMinSpeed, kMaxSpeed);
    }

    void nextFrame() {
      if (frame_ % speed_ == 0) {
        CRGB c;
        for (int i = 0; i < num_leds_; i++) {
          if (i % segment_length_ == 0) {
            c = randomColor();
          }
          leds_[i] = c; 
        }
      }
 
      frame_++;
    }

  private:
    int segment_length_, speed_, frame_;
};

};

#endif // STATIC_H
