# ESP32-2432S028R CYD LVGL Demo

A PlatformIO project demonstrating a display test demo using **LVGL 9** on the ESP32-2432S028R (CYD - Cheap Yellow Display) board with a direct ST7789 SPI driver (no TFT_eSPI).

## Features

- **LVGL 9**: Uses LVGL 9 display and widget APIs
- **Direct ST7789 driver**: Custom flush callback over SPI (no TFT_eSPI)
- **Demo sequence**: Startup screen, nested rectangles, display info, orientation test pattern, then loop

## Demo Sequence

1. **Startup Screen** (5 seconds): Board identification and driver information
2. **Nested Rectangles Animation**: Colorful rectangles growing inward (LVGL objects)
3. **Display Information**: Rotation, orientation, and display specs
4. **Orientation Test Pattern**: Corner markers, center cross, labels
5. **Loop**: Returns to step 2 and repeats

## Display Configuration

- **Resolution**: 240×320
- **Driver**: ST7789 via custom LVGL 9 display driver (SPI)
- **Color**: 16-bit RGB565; byte swap enabled for SPI (`LV_COLOR_16_SWAP 1`)
- **Buffers**: Double buffer, 40 lines each (~38 KB total), sized to fit ESP32 internal DRAM
- **SPI pins**: MISO 12, MOSI 13, SCLK 14, CS 15, DC 2, RST -1, Backlight 21

## Building and Uploading

```bash
pio run
pio run --target upload
pio device monitor
```

## Configuration Files

- **`include/lv_conf.h`**: LVGL configuration (color depth, memory, tick, features). Used via `LV_CONF_INCLUDE_SIMPLE` and `-I include` in `platformio.ini`.
- **`platformio.ini`**: Build flags include `LV_CONF_INCLUDE_SIMPLE`, `LV_KCONFIG_IGNORE`, and screen/pin defines.

## Customization

- **Colors**: Edit color constants and `LV_COLOR_MAKE(r,g,b)` in `main.cpp`.
- **Timing**: Adjust step durations in `loop()` (startup 5000 ms, rectangles/info/pattern 3000/5000 ms, etc.).
- **Buffer size**: Change `DRAW_BUF_HEIGHT` in `main.cpp`. Larger = fewer bands per frame (smoother) but more RAM; 40 lines chosen to avoid DRAM overflow on this board.

---

## Learnings from LVGL 9 Migration

Notes from upgrading to LVGL 9 and tuning performance on this board.

### 1. LVGL 9 display driver (no `lv_disp_drv_t`)

- Use **`lv_display_create(hor_res, ver_res)`**, then:
  - **`lv_display_set_buffers(disp, buf1, buf2, size_in_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL)`** — buffer size is in **bytes** (e.g. width × height × 2 for RGB565).
  - **`lv_display_set_flush_cb(disp, flush_cb)`**
  - **`lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565)`**
- Flush callback signature: **`void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)`** — `px_map` is raw RGB565 (uint8_t*), not `lv_color_t*`.
- Get the default screen with **`lv_display_get_screen_active(disp)`** (no separate `lv_scr_load` needed for the first screen).

### 2. Tick must be set for refresh and timers

- LVGL 9 does not use `LV_TICK_CUSTOM` from `lv_conf.h` for the tick source by default.
- **Call `lv_tick_set_cb(your_cb)`** after `lv_init()`, with a callback that returns current time in ms (e.g. **`(uint32_t)millis()`**).  
- Without this, the display refresh timer and other timers never advance; the first frame may show, then the screen appears stuck.

### 3. Config include and build (ESP32)

- Use **`LV_CONF_INCLUDE_SIMPLE`** and ensure `lv_conf.h` is on the include path (e.g. in `include/` with `-I include`). Avoid relying on **`LV_CONF_PATH`** as a bare macro if it expands without quotes and breaks `#include`.
- To avoid pulling in C headers when preprocessing LVGL’s assembly (e.g. Helium): **`#ifndef __ASSEMBLY__`** around **`#include <stdint.h>`** in `lv_conf.h`, and add **`LV_KCONFIG_IGNORE`** in build flags so `lv_conf_kconfig.h` (and thus `sdkconfig.h`) is not included during assembly preprocessing.

### 4. Display flush speed (SPI)

- **Avoid byte-by-byte `spi->transfer(byte)`** in the flush callback; it is very slow.
- Send data in **chunks** (e.g. 2 KB) with **`spi->transfer(chunk, len)`**. Copy from LVGL’s `px_map` into a small temporary buffer, then transfer that buffer so the SPI call does not overwrite LVGL’s buffer (some APIs overwrite the buffer with received data).
- Result: much faster, smoother updates.

### 5. Buffer size vs RAM (ESP32)

- Larger draw buffers = fewer bands per frame = smoother visual updates, but more RAM.
- **Static buffers** live in internal DRAM (`.dram0.bss`). On this board, **~40 lines** (double buffer) fits; 64 lines caused DRAM overflow (~8 KB over). If you have PSRAM, buffers could be moved there to allow larger sizes.

### 6. RGB565 byte order on SPI

- For 8-bit SPI interfaces, the display often expects the **two bytes of each RGB565 pixel swapped**.
- In **`lv_conf.h`** set **`LV_COLOR_16_SWAP 1`** so colors (and brightness) look correct; `0` can make colors look slightly off or darker.

### 7. Rectangles and widgets

- The demo draws rectangles as **LVGL objects** (`lv_obj_create` + styles). That is correct for LVGL; the main performance gain came from the **flush callback** (bulk SPI), not from changing how rectangles are drawn.

---

## Dependencies

- **LVGL** 9.x (e.g. `lvgl/lvgl@^9.5.0`)
- **Arduino-ESP32** (via `platform = espressif32`)
- No TFT_eSPI

## Hardware

- ESP32-2432S028R (CYD) with integrated 2.4" 240×320 ST7789 TFT, backlight on pin 21.
