#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Display configuration for ESP32-2432S028R
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_BL 21

// Create TFT display object (used only for hardware initialization)
TFT_eSPI tft = TFT_eSPI();

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10]; // 10 lines buffer

// Display flushing callback for LVGL
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// Global screen object
lv_obj_t *screen;

// Demo state variables
int demo_step = 0;
unsigned long last_demo_time = 0;
unsigned long step_start_time = 0;

// Color definitions (LVGL equivalents of TFT_eSPI colors)
const lv_color_t COLOR_BLACK = LV_COLOR_MAKE(0, 0, 0);
const lv_color_t COLOR_WHITE = LV_COLOR_MAKE(255, 255, 255);
const lv_color_t COLOR_RED = LV_COLOR_MAKE(255, 0, 0);
const lv_color_t COLOR_GREEN = LV_COLOR_MAKE(0, 255, 0);
const lv_color_t COLOR_BLUE = LV_COLOR_MAKE(0, 0, 255);
const lv_color_t COLOR_YELLOW = LV_COLOR_MAKE(255, 255, 0);
const lv_color_t COLOR_CYAN = LV_COLOR_MAKE(0, 255, 255);
const lv_color_t COLOR_MAGENTA = LV_COLOR_MAKE(255, 0, 255);
const lv_color_t COLOR_NAVY = LV_COLOR_MAKE(0, 0, 128);

// Function to clear screen and set background color
void fill_screen(lv_color_t color) {
    lv_obj_set_style_bg_color(screen, color, 0);
}

// Function to create startup message (equivalent to original setup display)
void show_startup_message() {
    fill_screen(COLOR_BLACK);
    
    // Create title label
    lv_obj_t *title_label = lv_label_create(screen);
    lv_label_set_text(title_label, "ESP32-2432S028R (CYD)");
    lv_obj_set_style_text_color(title_label, COLOR_WHITE, 0);
    lv_obj_set_pos(title_label, 20, 30);
    
    // Create subtitle
    lv_obj_t *subtitle_label = lv_label_create(screen);
    lv_label_set_text(subtitle_label, "Display Test");
    lv_obj_set_style_text_color(subtitle_label, COLOR_WHITE, 0);
    lv_obj_set_pos(subtitle_label, 20, 60);
    
    // Create resolution info
    lv_obj_t *res_label = lv_label_create(screen);
    lv_label_set_text(res_label, "Resolution: 240x320");
    lv_obj_set_style_text_color(res_label, COLOR_WHITE, 0);
    lv_obj_set_pos(res_label, 20, 100);
    
    // Create driver info
    lv_obj_t *driver_label = lv_label_create(screen);
    lv_label_set_text(driver_label, "Driver: ST7789");
    lv_obj_set_style_text_color(driver_label, COLOR_WHITE, 0);
    lv_obj_set_pos(driver_label, 20, 120);
}

// Function to create nested rectangles animation
void show_nested_rectangles() {
    static int current_rect = 0;
    static unsigned long last_rect_time = 0;
    static bool animation_complete = false;
    
    // Initialize animation
    if(current_rect == 0 && millis() - step_start_time < 100) {
        fill_screen(COLOR_BLACK);
        animation_complete = false;
    }
    
    // Draw rectangles progressively
    if(!animation_complete && millis() - last_rect_time > 200) {
        lv_color_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA};
        int numColors = sizeof(colors) / sizeof(colors[0]);
        
        if(current_rect < SCREEN_WIDTH/2) {
            // Create rectangle object
            lv_obj_t *rect = lv_obj_create(screen);
            lv_obj_remove_style_all(rect);
            
            int colorIndex = (current_rect / 20) % numColors;
            lv_obj_set_style_bg_color(rect, colors[colorIndex], 0);
            lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
            lv_obj_set_style_border_width(rect, 0, 0);
            
            lv_obj_set_pos(rect, current_rect, current_rect);
            lv_obj_set_size(rect, SCREEN_WIDTH - (current_rect * 2), SCREEN_HEIGHT - (current_rect * 2));
            
            current_rect += 20;
            last_rect_time = millis();
        } else {
            animation_complete = true;
            current_rect = 0; // Reset for next time
        }
    }
}

// Function to show display information
void show_display_info() {
    fill_screen(COLOR_BLACK);
    
    // Main title
    lv_obj_t *main_label = lv_label_create(screen);
    lv_label_set_text(main_label, "Display Working!");
    lv_obj_set_style_text_color(main_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(main_label, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(main_label, 10, 30);
    
    // Rotation info
    lv_obj_t *rot_label = lv_label_create(screen);
    lv_label_set_text(rot_label, "Rotation: 2");
    lv_obj_set_style_text_color(rot_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(rot_label, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(rot_label, 10, 60);
    
    // Orientation
    lv_obj_t *orient_label = lv_label_create(screen);
    lv_label_set_text(orient_label, "Portrait Flip");
    lv_obj_set_style_text_color(orient_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(orient_label, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(orient_label, 10, 90);
    
    // Display dimensions
    lv_obj_t *width_label = lv_label_create(screen);
    lv_label_set_text(width_label, "Width: 240");
    lv_obj_set_style_text_color(width_label, COLOR_WHITE, 0);
    lv_obj_set_pos(width_label, 10, 130);
    
    lv_obj_t *height_label = lv_label_create(screen);
    lv_label_set_text(height_label, "Height: 320");
    lv_obj_set_style_text_color(height_label, COLOR_WHITE, 0);
    lv_obj_set_pos(height_label, 10, 150);
    
    lv_obj_t *driver_label = lv_label_create(screen);
    lv_label_set_text(driver_label, "Driver: ST7789");
    lv_obj_set_style_text_color(driver_label, COLOR_WHITE, 0);
    lv_obj_set_pos(driver_label, 10, 170);
}

// Function to show orientation test pattern
void show_orientation_pattern() {
    fill_screen(COLOR_NAVY);
    
    // Corner rectangles
    lv_obj_t *corners[4];
    lv_color_t corner_colors[] = {COLOR_GREEN, COLOR_RED, COLOR_YELLOW, COLOR_CYAN};
    
    // Top-left
    corners[0] = lv_obj_create(screen);
    lv_obj_remove_style_all(corners[0]);
    lv_obj_set_style_bg_color(corners[0], corner_colors[0], 0);
    lv_obj_set_style_bg_opa(corners[0], LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(corners[0], 0, 0);
    lv_obj_set_pos(corners[0], 0, 0);
    lv_obj_set_size(corners[0], 40, 40);
    
    // Top-right
    corners[1] = lv_obj_create(screen);
    lv_obj_remove_style_all(corners[1]);
    lv_obj_set_style_bg_color(corners[1], corner_colors[1], 0);
    lv_obj_set_style_bg_opa(corners[1], LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(corners[1], 0, 0);
    lv_obj_set_pos(corners[1], SCREEN_WIDTH-40, 0);
    lv_obj_set_size(corners[1], 40, 40);
    
    // Bottom-left
    corners[2] = lv_obj_create(screen);
    lv_obj_remove_style_all(corners[2]);
    lv_obj_set_style_bg_color(corners[2], corner_colors[2], 0);
    lv_obj_set_style_bg_opa(corners[2], LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(corners[2], 0, 0);
    lv_obj_set_pos(corners[2], 0, SCREEN_HEIGHT-40);
    lv_obj_set_size(corners[2], 40, 40);
    
    // Bottom-right
    corners[3] = lv_obj_create(screen);
    lv_obj_remove_style_all(corners[3]);
    lv_obj_set_style_bg_color(corners[3], corner_colors[3], 0);
    lv_obj_set_style_bg_opa(corners[3], LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(corners[3], 0, 0);
    lv_obj_set_pos(corners[3], SCREEN_WIDTH-40, SCREEN_HEIGHT-40);
    lv_obj_set_size(corners[3], 40, 40);
    
    // Center cross - vertical line
    lv_obj_t *v_line = lv_obj_create(screen);
    lv_obj_remove_style_all(v_line);
    lv_obj_set_style_bg_color(v_line, COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(v_line, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(v_line, 0, 0);
    lv_obj_set_pos(v_line, SCREEN_WIDTH/2-5, 0);
    lv_obj_set_size(v_line, 10, SCREEN_HEIGHT);
    
    // Center cross - horizontal line
    lv_obj_t *h_line = lv_obj_create(screen);
    lv_obj_remove_style_all(h_line);
    lv_obj_set_style_bg_color(h_line, COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(h_line, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(h_line, 0, 0);
    lv_obj_set_pos(h_line, 0, SCREEN_HEIGHT/2-5);
    lv_obj_set_size(h_line, SCREEN_WIDTH, 10);
    
    // Corner labels
    lv_obj_t *tl_label = lv_label_create(screen);
    lv_label_set_text(tl_label, "TL");
    lv_obj_set_style_text_color(tl_label, COLOR_BLACK, 0);
    lv_obj_set_pos(tl_label, 5, 15);
    
    lv_obj_t *tr_label = lv_label_create(screen);
    lv_label_set_text(tr_label, "TR");
    lv_obj_set_style_text_color(tr_label, COLOR_BLACK, 0);
    lv_obj_set_pos(tr_label, SCREEN_WIDTH-25, 15);
    
    lv_obj_t *bl_label = lv_label_create(screen);
    lv_label_set_text(bl_label, "BL");
    lv_obj_set_style_text_color(bl_label, COLOR_BLACK, 0);
    lv_obj_set_pos(bl_label, 5, SCREEN_HEIGHT-25);
    
    lv_obj_t *br_label = lv_label_create(screen);
    lv_label_set_text(br_label, "BR");
    lv_obj_set_style_text_color(br_label, COLOR_BLACK, 0);
    lv_obj_set_pos(br_label, SCREEN_WIDTH-25, SCREEN_HEIGHT-25);
    
    // Center labels
    lv_obj_t *center_label1 = lv_label_create(screen);
    lv_label_set_text(center_label1, "Portrait Flip");
    lv_obj_set_style_text_color(center_label1, COLOR_BLACK, 0);
    lv_obj_set_style_text_font(center_label1, &lv_font_montserrat_14, 0);
    lv_obj_set_style_bg_color(center_label1, COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(center_label1, LV_OPA_COVER, 0);
    lv_obj_set_pos(center_label1, SCREEN_WIDTH/2-60, SCREEN_HEIGHT/2-20);
    
    lv_obj_t *center_label2 = lv_label_create(screen);
    lv_label_set_text(center_label2, "MODE");
    lv_obj_set_style_text_color(center_label2, COLOR_BLACK, 0);
    lv_obj_set_style_text_font(center_label2, &lv_font_montserrat_14, 0);
    lv_obj_set_style_bg_color(center_label2, COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(center_label2, LV_OPA_COVER, 0);
    lv_obj_set_pos(center_label2, SCREEN_WIDTH/2-30, SCREEN_HEIGHT/2+5);
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("ESP32-2432S028R Display Test");

    // Initialize backlight pin
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // Turn on backlight

    // Initialize TFT display
    tft.init();
    tft.setRotation(2); // Portrait mode (240x320)
    tft.fillScreen(TFT_BLACK);

    // Print display dimensions
    Serial.printf("Display dimensions: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    // Initialize LVGL
    lv_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Create main screen
    screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    // Show startup message (equivalent to original setup display)
    show_startup_message();

    Serial.println("LVGL initialization complete!");
    
    // Set initial timing
    step_start_time = millis();
    last_demo_time = millis();
}

void loop() {
    unsigned long current_time = millis();
    
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Demo sequence timing (matching original demo)
    switch(demo_step) {
        case 0: // Show startup message for 5 seconds
            if(current_time - step_start_time > 5000) {
                demo_step = 1;
                step_start_time = current_time;
                // Clear all objects for next step
                lv_obj_clean(screen);
            }
            break;
            
        case 1: // Show nested rectangles animation
            show_nested_rectangles();
            if(current_time - step_start_time > 3000) { // Allow time for animation + 1 second delay
                demo_step = 2;
                step_start_time = current_time;
                lv_obj_clean(screen);
            }
            break;
            
        case 2: // Show display information for 3 seconds
            if(current_time - step_start_time < 100) { // Only create once
                show_display_info();
            }
            if(current_time - step_start_time > 3000) {
                demo_step = 3;
                step_start_time = current_time;
                lv_obj_clean(screen);
            }
            break;
            
        case 3: // Show orientation test pattern for 3 seconds
            if(current_time - step_start_time < 100) { // Only create once
                show_orientation_pattern();
            }
            if(current_time - step_start_time > 3000) {
                demo_step = 1; // Loop back to rectangles animation
                step_start_time = current_time;
                lv_obj_clean(screen);
            }
            break;
    }
    
    delay(5); // Small delay to prevent watchdog issues
}
