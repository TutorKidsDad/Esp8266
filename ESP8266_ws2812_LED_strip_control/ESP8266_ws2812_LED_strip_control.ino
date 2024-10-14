#include <FastLED.h>

#define NUM_LEDS 8          // Number of LEDs in your strip
#define DATA_PIN D7           // Data pin connected to the WS2812 strip
#define ANALOG_PIN A0        // Analog pin to read values for color control
#define BRIGHTNESS 128       // Brightness (0-255)

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS); // Initialize the LED strip
  FastLED.setBrightness(BRIGHTNESS);                     // Set initial brightness
  Serial.begin(115200);
}

void loop() {
  int analogValue = analogRead(ANALOG_PIN); // Read the analog value (0-1023)
  Serial.println(analogValue);
  delay(10);     // Delay for smoother color changes

  // Define the color ranges
  CRGB colorRed = CRGB(255, 0, 0);
  //CRGB colorYellow = CRGB(255, 255, 0);
  CRGB colorOrange = CRGB(255, 165, 0); // Red: 255, Green: 165, Blue: 0
  CRGB colorGreen = CRGB(0, 255, 0);

  // Map the analog value to different color ranges
  CRGB color;
  if (analogValue >= 0 && analogValue <= 300) {
    color = colorRed;
  } else if (analogValue > 300 && analogValue <= 650) {
    color = colorOrange;
  } else {
    color = colorGreen;
  }

  // Fill the LED strip with the determined color
  fill_solid(leds, NUM_LEDS, color);

  FastLED.show(); // Display the updated colors on the LED strip
  delay(100);     // Delay for smoother color changes
}
