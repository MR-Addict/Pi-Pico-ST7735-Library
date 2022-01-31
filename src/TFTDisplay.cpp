#include <cstdio>

#include "imageHeader/girl.h"
#include "imageHeader/lamp.h"
#include "lib/ST7735/ST7735.h"
#include "pico/stdlib.h"

// Some useful colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0

int main() {
    // SCL, SDA, CS, DC, width, height, frequency,SPI Port
    ST7735 TFT(19, 19, 20, 21, 128, 160, 15000000, spi0);
    // Print a string
    uint8_t string01[] = "We think you will love these projrcts";
    TFT.print(0, 0, string01, GREEN);
    // Draw a rectangles
    TFT.drawRectangle(0, 50, 128, 20, BLUE);
    TFT.drawFilledRectangle(3, 53, 122, 14, BLUE);
    // Draw circles
    TFT.drawFilledCircle(34, 60, 5, RED);
    TFT.drawFilledCircle(94, 60, 5, RED);
    // Draw an image
    TFT.drawImage(0, 96, 128, 64, girl);
    // TFT.drawImage(0, 0, 128, 160, lamp);
    while (true) {
        tight_loop_contents();
    }
    return 0;
}