#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H

#include "animation.h"

namespace FuelGauge {

constexpr int kNumFuelGaugeFrames = 100;
constexpr int kPulseSpeed = 5;
constexpr int kNumADCReadings = 100;
constexpr float kMinV = 2.5;
constexpr float kMaxV = 4.2;

constexpr int kNumCurrentlyChargingFrames = 220;
constexpr float kFillSpeed = 0.75;
constexpr int kFillPauseLength = 15;

class FuelGaugeAnimation : public Animation {
  public:
    FuelGaugeAnimation(CRGB* leds, int num_leds, float percent_full) :
             Animation(leds, num_leds, kNumFuelGaugeFrames),
             percent_full_(percent_full), angle_(0) {};

    void generateNextFrame() {
      int value = static_cast<int>(abs(sin(radians(angle_))) * 100) + 155;

      int num_leds_lit = percent_full_ * num_leds_;
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (i < num_leds_lit) ? CHSV(0, 255, value) : CHSV (0, 0, 128);
      }

      angle_ = (angle_ + kPulseSpeed) % 360;
    };

  private:
    float percent_full_;
    int angle_;
};

class CurrentlyChargingAnimation : public Animation {
  public:
    CurrentlyChargingAnimation(CRGB* leds, int num_leds) :
                     Animation(leds, num_leds, kNumCurrentlyChargingFrames),
                     fill_level_(0.0) {};

    void generateNextFrame() {
      for (int i = 0; i < num_leds_; i++) {
        leds_[i] = (i < static_cast<int>(fill_level_)) ?
                              CHSV(0, 255, 255) : CHSV(0, 0, 128);
      }
      fill_level_ += kFillSpeed;
      if (static_cast<int>(fill_level_) > (num_leds_ + kFillPauseLength)) {
        fill_level_ = 0.0;
      }
    };

  private:
    float fill_level_;
};


Animation* buildFuelGaugeAnimation(int adc_pin, int usb_power_detection_pin,
                                   CRGB* leds, int num_leds) {
  // First check if the USB power is plugged in.  We can't really get good
  // readings if it's plugged into power and currently charging.  If USB
  // power is plugged in, we just show an animation that indicates that.
  // TODO: Add an additional sense line to see if the lipo charger is done
  //       charging or not (the fully-charged status LED on it)
  if (digitalRead(usb_power_detection_pin)) {
    return new CurrentlyChargingAnimation(leds, num_leds);
  }

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
