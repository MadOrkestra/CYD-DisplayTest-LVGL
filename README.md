# ESP32-2432S028R CYD LVGL Demo

A PlatformIO project demonstrating the **exact same display demo** as the original TFT_eSPI version, but reimplemented using LVGL (Light and Versatile Graphics Library) on the ESP32-2432S028R (CYD - Cheap Yellow Display) board with ST7789 driver.

## Features

- **Identical Visual Demo**: Recreates the exact same display sequence as the original
- **LVGL Backend**: Uses modern LVGL framework instead of direct TFT drawing
- **Same Timing**: Maintains original demo timing and animations
- **No Interactive Elements**: Pure display demo without buttons or sliders

## Demo Sequence

The demo replicates the original sequence exactly:

1. **Startup Screen** (5 seconds): Board identification and driver information
2. **Nested Rectangles Animation**: Colorful animated rectangles growing inward
3. **Display Information**: Current rotation, orientation, and display specs  
4. **Orientation Test Pattern**: Corner markers, center cross, and orientation labels
5. **Loop**: Returns to step 2 and repeats indefinitely

## Display Configuration

This project uses LVGL with a custom display driver for the ST7789 display:

- Resolution: 240x320 pixels
- Driver: ST7789 with LVGL integration
- Orientation: Portrait (Rotation 2)
- Color depth: 16-bit (RGB565)
- Buffer: 10-line buffer for smooth rendering
- SPI Pins:
  - MISO: 12
  - MOSI: 13
  - SCLK: 14
  - CS: 15
  - DC: 2
  - RST: Not used (-1)
  - Backlight: 21

## Implementation Details

- **Display Driver**: Custom LVGL flush callback using TFT_eSPI for hardware access
- **Graphics Rendering**: All drawing operations converted to LVGL objects and styles
- **Animation System**: Recreated nested rectangle animation using LVGL objects
- **Text Rendering**: Converted TFT_eSPI text calls to LVGL labels
- **Color Mapping**: Direct translation of TFT color constants to LVGL colors

## Hardware Requirements

- ESP32-2432S028R (CYD - Cheap Yellow Display) board
- 2.8" ST7789 TFT display (240x320)
- Built-in backlight control (pin 21)

## Dependencies

- LVGL 8.4.0+ (GUI library)
- TFT_eSPI 2.5.43+ (Hardware abstraction only)

## Configuration Files

- `lv_conf.h`: LVGL configuration optimized for ESP32
- Memory allocation: 48KB heap for LVGL
- Tick source: Arduino millis() function
- 16-bit color depth for ST7789 compatibility

## Building and Uploading

```bash
# Build the project
pio run

# Upload to the ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

## LVGL UI Structure

The demo recreates the original display sequence using LVGL components:

1. **Startup Labels**: Board identification and driver information
2. **Animated Rectangles**: LVGL objects with progressive positioning and coloring
3. **Information Display**: Multiple labels showing display properties  
4. **Test Pattern**: Colored corner objects, center cross lines, and orientation labels

## Customization

### Changing Colors

Modify the color constants at the top of `main.cpp`:

- `COLOR_BLACK`, `COLOR_WHITE`, `COLOR_RED`, etc.
- Use `LV_COLOR_MAKE(r, g, b)` macro for custom colors

### Modifying Timing

Adjust timing values in the `loop()` function:

- Startup message: 5000ms
- Rectangle animation: 3000ms  
- Info display: 3000ms
- Test pattern: 3000ms

### Display Orientation

Change orientation by modifying `tft.setRotation(2)` in setup():

- 0: Portrait, USB at top
- 1: Landscape, USB at left  
- 2: Portrait flipped, USB at bottom (current)
- 3: Landscape flipped, USB at right
