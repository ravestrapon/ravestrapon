#ifndef BLOCKIFY_H
#define BLOCKIFY_H

#include "animation.h"

namespace Blockify {

constexpr int kMaxBlocks = 5;
constexpr int kMinBlocks = 2;

// Note: For this animation the speed is the number of frames between
// color changes.  This means a higher "speed" actually results in the
// colors changing more slowly.
constexpr int kMinSpeed = 5;
constexpr int kMaxSpeed = 10;

class BlockifyAnimation : public Animation {
  public:
    BlockifyAnimation(CRGB* leds, int num_leds, int num_frames) :
            Animation(leds, num_leds, num_frames) {
      fillRandomContrastingColors(c1_, c2_);
      int num_blocks = random(kMinBlocks, kMaxBlocks);
      block_size_ = num_leds_ / num_blocks;
      speed_ = random(kMinSpeed, kMaxSpeed);
    }

    void generateNextFrame() {
      // If it's time to redraw, swap the colors and redraw
      if (frame_ % speed_ == 0) {
        CRGB c_temp = c1_;
        c1_ = c2_;
        c2_ = c_temp;

        for (int i = 0; i < num_leds_; i++) {
          leds_[i] = ((i / block_size_) % 2) ? c1_ : c2_;
        }
      }
    }

  private:
    CRGB c1_, c2_;
    int speed_, block_size_;
};

};

#endif // BLOCKIFY_H
