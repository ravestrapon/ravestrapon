#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H

#include <FastLED.h>

#define FUEL_GAUGE_NUM_READINGS 100

#define FUEL_DISPLAY_FILL_TIME_MS 350
#define FUEL_DISPLAY_NUM_PULSES 3
#define FUEL_DISPLAY_PULSE_TIME_MS 1800
#define FUEL_DISPLAY_PULSE_STEPS 100

#define RED_HUE 0
#define ORANGE_HUE 32
#define GREEN_HUE 96
#define FILL_VALUE 180

#define FUEL_DISPLAY_INTERPULSE_DELAY_MS 200
#define FUEL_DISPLAY_PULSE_VALUE_SWING (255 - FILL_VALUE)

#define MIN_V 2.5
#define MAX_V 4.2

int fuelGaugeHue(int i, int num_leds) {
  if (i < num_leds * 0.25)
    return RED_HUE;
  else if (i < num_leds * 0.60)
    return ORANGE_HUE;
  return GREEN_HUE;
}

void displayFuelLevel(CRGB* leds, int num_leds, float fuel_perc) {
  int fill_level = num_leds * fuel_perc;
  int fill_frame_delay = (FUEL_DISPLAY_FILL_TIME_MS / fill_level);
  int pulse_frame_delay = (FUEL_DISPLAY_PULSE_TIME_MS /
                           (FUEL_DISPLAY_PULSE_STEPS * 2));


  // First "fill" up to the current fuel level
  for (int current_fill = 0; current_fill < fill_level; current_fill++) {
    for (int i = 0; i < num_leds; i++) {
      int hue = fuelGaugeHue(i, num_leds);
      leds[i] = (i <= current_fill) ? CHSV(hue, 255, FILL_VALUE) : CRGB(0, 0, 0);
    }
    FastLED.show();
    delay(fill_frame_delay);
  }
  
  // Now "pulse a few times once filled
  for (int pulse = 0; pulse < FUEL_DISPLAY_NUM_PULSES; pulse++) {
    delay(FUEL_DISPLAY_INTERPULSE_DELAY_MS);
    float limit = (pulse == FUEL_DISPLAY_NUM_PULSES - 1) ? PI * 1.5 : TWO_PI;
    for (float a = 0.0; a <= limit; a += (TWO_PI / FUEL_DISPLAY_PULSE_STEPS)) {
      int v = FILL_VALUE + (FUEL_DISPLAY_PULSE_VALUE_SWING * abs(sin(a)));
      for (int i = 0; i < num_leds; i++) {
        int hue = fuelGaugeHue(i, num_leds);
        leds[i] = (i <= fill_level) ? CHSV(hue, 255, v) : CRGB(0, 0, 0);
      }
      FastLED.show();
      delay(pulse_frame_delay);
    }
  }

  // Finally "retract" before resuming normal function
  for (int current_fill = fill_level; current_fill >= 0; current_fill--) {
    for (int i = 0; i < num_leds; i++) {
      int hue = fuelGaugeHue(i, num_leds);
      leds[i] = (i <= current_fill) ? CHSV(hue, 255, FILL_VALUE) : CRGB(0, 0, 0);
    }
    FastLED.show();
    delay(fill_frame_delay);
  }
}

void displayFuelGauge(int adc_pin, CRGB* leds, int num_leds) {
  // Take the average of many readings on the adc for smoothing
  int64_t readings_total = 0;
  for (int i = 0; i < FUEL_GAUGE_NUM_READINGS; i++) {
    readings_total += analogRead(adc_pin);
  }
  int reading = readings_total / FUEL_GAUGE_NUM_READINGS;

  // Convert that ADC reading to voltage.  The ADC as 1024 values
  // and a 2.56V reference voltage.  This PCB has the ADC wired to
  // a 1/2 voltage divider as well, to increase the sensing range.
  float voltage = reading / 1024.0 * 2.56 * 2.0;

  // Compute what percentage of the battery remains
  // TODO: This could definitely be updated, the power remaining is
  //       definitely not proportional linearly to the voltage, but that's
  //       the simple assumption made here.
  float perc = (voltage - MIN_V) / (MAX_V - MIN_V);

  // Using that value, display the battery level to the user.
  displayFuelLevel(leds, num_leds, perc);
}


#endif // FUEL_GAUGE_H
