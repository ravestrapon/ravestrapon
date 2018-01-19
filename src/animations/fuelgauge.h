#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H

#include "animation.h"

namespace FuelGauge {

constexpr int kNumFrames = 30;
constexpr int kNumADCReadings = 100;
constexpr float kMinV = 2.5;
constexpr float kMaxV = 4.2;

class FuelGaugeAnimation : public Animation {
  public:
    FuelGaugeAnimation(CRGB* leds, int num_leds, float percent_full) :
             Animation(leds, num_leds, kNumFrames),
             percent_full_(percent_full) {};

    void generateNextFrame() {
      int num_leds_lit = percent_full_ * num_leds_;
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (i < num_leds_lit) ? CRGB(255, 0, 0) : CRGB (0, 0, 0);
      }
    };

  private:
    float percent_full_;
};

Animation* buildFuelGaugeAnimation(int adc_pin, CRGB* leds, int num_leds) {
  // Take the average of many readings on the adc for smoothing
  int64_t readings_total = 0;
  for (int i = 0; i < kNumADCReadings; i++) {
    readings_total += analogRead(adc_pin);
  }
  int reading = readings_total / kNumADCReadings;

  // Convert that ADC reading to voltage.  The ADC as 1024 values
  // and a 2.56V reference voltage.  This PCB has the ADC wired to
  // a 1/2 voltage divider as well, to increase the sensing range.
  float voltage = reading / 1024.0 * 2.56 * 2.0;

  // Compute what percentage of the battery remains
  // TODO: This could definitely be updated, the power remaining is
  //       definitely not proportional linearly to the voltage, but that's
  //       the simple assumption made here.
  float percent_full = (voltage - kMinV) / (kMaxV - kMinV);

  // Using that value, display the battery level to the user.
  return new FuelGaugeAnimation(leds, num_leds, percent_full);
}

};

#endif // FUEL_GAUGE_H
