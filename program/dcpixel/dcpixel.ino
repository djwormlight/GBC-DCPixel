#include <WS2812.h>

#define NUM_LEDS_X 8
#define NUM_LEDS_Y 8
#define COLOR_PER_LEDS 3
#define NUM_LEDS_TOTAL (NUM_LEDS_X * NUM_LEDS_Y)
#define NUM_BYTES (NUM_LEDS_TOTAL * COLOR_PER_LEDS)

// 0 = 100% brightness, 1 = 50%, 2 = 25%, 3 = 12%, 4 = 6%, 5 = 3%
#define BRIGHTNESS_FACTOR 4

__xdata uint8_t image[NUM_BYTES];
__xdata uint8_t ledData[NUM_BYTES];

void convert_rgb_to_grb(uint8_t* imageBuffer) {  
  for (size_t i = 0; i < NUM_BYTES; i += COLOR_PER_LEDS) {
    uint8_t red = imageBuffer[i];
    imageBuffer[i] = imageBuffer[i+1];
    imageBuffer[i+1] = red;
  }
}

void applyBrightnessToImage(uint8_t brightnessShift, uint8_t* imageBuffer) {
  for (size_t i = 0; i < NUM_BYTES; ++i) {
    imageBuffer[i] = imageBuffer[i] >> brightnessShift;
  }
}

void fillRainbowStartupImage(uint8_t* imageBuffer) {
  const uint8_t rainbowColors[7][3] = {
    { 0xFF, 0x00, 0x00 }, // Red
    { 0xFF, 0xA5, 0x00 }, // Orange
    { 0xFF, 0xFF, 0x00 }, // Yellow
    { 0x00, 0x80, 0x00 }, // Green
    { 0x00, 0x00, 0xFF }, // Blue
    { 0x4B, 0x00, 0x82 }, // Indigo
    { 0x94, 0x00, 0xD3 }  // Violet
  };

  size_t totalRainbowColors = sizeof(rainbowColors) / sizeof(rainbowColors[0]);

  for (size_t i = 0; i < NUM_BYTES; i += COLOR_PER_LEDS) {
    size_t colorIndex = (i / COLOR_PER_LEDS) % totalRainbowColors;

    imageBuffer[i] = rainbowColors[colorIndex][0];
    imageBuffer[i+1] = rainbowColors[colorIndex][1];
    imageBuffer[i+2] = rainbowColors[colorIndex][2];
  }

  applyBrightnessToImage(BRIGHTNESS_FACTOR, imageBuffer);

  convert_rgb_to_grb(imageBuffer);
}

void fillMainImage(uint8_t* imageBuffer) {
  const uint8_t mainImage[NUM_BYTES] = {
    // Image: ./pic.png (8, 8)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0x23, 0x34, 0xAE, 0x23, 0x34, 0xAE, 0x23, 0x34, 0xAE, 0x23, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xAE, 0x23, 0x34, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xF0, 0x87, 0x87, 0xAE, 0x23, 0x34, 0x00, 0x00, 0x00,
    0xAE, 0x23, 0x34, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xF0, 0x87, 0x87, 0xAE, 0x23, 0x34,
    0xAE, 0x23, 0x34, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x3B, 0x3B, 0xE8, 0x3B, 0x3B, 0xAE, 0x23, 0x34,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xCC, 0xCC, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC,
    0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  memcpy(imageBuffer, mainImage, NUM_BYTES);

  applyBrightnessToImage(BRIGHTNESS_FACTOR, imageBuffer);

  convert_rgb_to_grb(imageBuffer);
}

void animateImageWithRowWipe(uint8_t* imageBuffer, uint8_t* ledBuffer) {
  memset(ledData, 0, sizeof(ledData));

  for (size_t row = 0; row < NUM_LEDS_Y; ++row) {
    size_t rowStartIndex = (row * NUM_LEDS_X) * COLOR_PER_LEDS;

    memcpy(&ledBuffer[rowStartIndex], &imageBuffer[rowStartIndex], NUM_LEDS_X * COLOR_PER_LEDS);

    neopixel_show_P1_4(ledBuffer, NUM_BYTES);

    delay(1000);
  }
}

void animateImageWithPowerEfficiency(uint8_t* imageBuffer, uint8_t* ledBuffer) {
  const int32_t onDuration = 3;
  const int32_t offDuration = 7;

  memcpy(ledBuffer, imageBuffer, NUM_BYTES);

  neopixel_show_P1_4(ledBuffer, NUM_BYTES);
  delay(onDuration);

  memset(ledBuffer, 0, NUM_BYTES);
  neopixel_show_P1_4(ledBuffer, NUM_BYTES);
  delay(offDuration);
}

void setup() {
  pinMode(14, OUTPUT);

  fillRainbowStartupImage(image);

  animateImageWithRowWipe(image, ledData);

  fillMainImage(image);
}

void loop() {
  animateImageWithPowerEfficiency(image, ledData);
}