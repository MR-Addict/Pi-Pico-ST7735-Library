#include <cstdio>

#include "ST7735.h"
#include "font/Dialog_bold_10.h"

uint16_t CRGB(uint32_t color_rgb) {
    uint8_t r = (color_rgb & 0xff0000) >> 16;
    uint8_t g = (color_rgb & 0xff00) >> 8;
    uint8_t b = (color_rgb & 0xff);
    return CRGB(r, g, b);
}

uint16_t CRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t R5 = (r * 249 + 1014) >> 11;
    uint8_t G6 = (g * 253 + 505) >> 10;
    uint8_t B5 = (b * 249 + 1014) >> 11;
    return ((uint16_t)(R5 << 11) | (uint16_t)(G6 << 5) | B5);
}

void ST7735::swap(uint8_t* x1, uint8_t* x2) {
    uint8_t temp = *x1;
    *x1 = *x2, *x2 = temp;
}

void ST7735::ST7735_set_pin(uint8_t pin, bool status) {
    asm volatile("nop \n nop \n nop");
    gpio_put(pin, status);
    asm volatile("nop \n nop \n nop");
}

void ST7735::write_cmd(uint8_t cmd) {
    // DC Pin Low For Tramsmit Command
    ST7735_set_pin(ST7735_DC_PIN, false);
    ST7735_set_pin(ST7735_CS_PIN, false);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    ST7735_set_pin(ST7735_CS_PIN, true);
}

void ST7735::write_data(uint8_t data) {
    // DC Pin High For Tramsmit Data
    ST7735_set_pin(ST7735_DC_PIN, true);
    ST7735_set_pin(ST7735_CS_PIN, false);
    spi_write_blocking(SPI_PORT, &data, 1);
    ST7735_set_pin(ST7735_CS_PIN, true);
}

void ST7735::hardware_init() {
    spi_init(SPI_PORT, FREQUENCY);
    gpio_set_function(ST7735_SCL_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7735_SDA_PIN, GPIO_FUNC_SPI);

    gpio_init(ST7735_CS_PIN);
    gpio_init(ST7735_DC_PIN);
    gpio_set_dir(ST7735_CS_PIN, GPIO_OUT);
    gpio_set_dir(ST7735_DC_PIN, GPIO_OUT);
    gpio_put(ST7735_CS_PIN, true);
    gpio_put(ST7735_DC_PIN, true);
}

void ST7735::ST7735_init() {
    // Sleep Out, and wait 120ms for stablization
    write_cmd(SET_SLEEP | 0x01);
    sleep_ms(120);
    // Clear Display and Turn On Display
    write_cmd(SET_DISPLAY | 0x01);
    sleep_ms(120);
    // Set Partical Display
    write_cmd(SET_PARTICAL);
    write_cmd(SET_PARTICAL_ADDRESS);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(HEIGHT);
    // Turn Inverse OFF
    write_cmd(SET_INVERSE | 0x00);
    // Turn Idle Mode OFF
    write_cmd(SET_IDLE | 0x00);
    // Turn Tearing Effects OFF
    write_cmd(SET_TEARING | 0x00);
    // Set Memory Access Mode 0xC0:Mirror X, Mirror Y and Mirror Virtical, RGB
    // Order
    write_cmd(SET_MEMORY_ACCESS);
    write_data(0xD0);
    // Set pixel Mode 0x05:16-bit/pixel
    write_cmd(SET_PIXEL_MODE);
    write_data(0x05);
}

void ST7735::set_windows(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    write_cmd(SET_COL_ADDRESS);
    write_data(0x00);
    write_data(x);
    write_data(0x00);
    write_data(x + width - 1);
    write_cmd(SET_ROW_ADDRESS);
    write_data(0x00);
    write_data(y);
    write_data(0x00);
    write_data(y + height - 1);
}

ST7735::ST7735(uint8_t scl,
               uint8_t sda,
               uint8_t cs,
               uint8_t dc,
               uint8_t width,
               uint8_t height,
               uint32_t freq,
               spi_inst_t* spi) {
    WIDTH = width, HEIGHT = height;
    ST7735_DC_PIN = dc, ST7735_CS_PIN = cs;
    ST7735_SDA_PIN = sda, ST7735_SCL_PIN = scl;
    FREQUENCY = freq;
    SPI_PORT = spi;
    myFont = &Dialog_bold_10;
    hardware_init();
    ST7735_init();
    clear();
}

ST7735::~ST7735() {}

void ST7735::clear() {
    fill(0x00);
}

void ST7735::fill(uint16_t color) {
    drawFilledRectangle(0, 0, WIDTH, HEIGHT, color);
}

void ST7735::drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x < WIDTH && y < HEIGHT) {
        set_windows(x, y, 1, 1);
        write_cmd(MEMORY_WRITE);
        write_data(color >> 8);
        write_data(color & 0xFF);
    }
}

void ST7735::drawFastHLine(uint8_t x,
                           uint8_t y,
                           uint8_t width,
                           uint16_t color) {
    drawFilledRectangle(x, y, width, 1, color);
}

void ST7735::drawFastVLine(uint8_t x,
                           uint8_t y,
                           uint8_t height,
                           uint16_t color) {
    drawFilledRectangle(x, y, 1, height, color);
}

void ST7735::drawLine(uint8_t x1,
                      uint8_t y1,
                      uint8_t x2,
                      uint8_t y2,
                      uint16_t color) {
    if (x1 > x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    float m = (float)(y2 - y1) / (float)(x2 - x1);
    for (uint8_t x = x1; x <= x2; x++) {
        float y = m * (float)(x - x2) + (float)y2;
        drawPixel(x, y, color);
    }
}

void ST7735::drawRectangle(uint8_t x,
                           uint8_t y,
                           uint8_t width,
                           uint8_t height,
                           uint16_t color) {
    drawFastHLine(x, y, width, color);
    drawFastHLine(x, y + height - 1, width, color);
    drawFastVLine(x, y, height, color);
    drawFastVLine(x + width - 1, y, height, color);
}

void ST7735::drawFilledRectangle(uint8_t x,
                                 uint8_t y,
                                 uint8_t width,
                                 uint8_t height,
                                 uint16_t color) {
    if (x + width > WIDTH)
        width = WIDTH - x - 1;
    if (y + height > HEIGHT)
        height = HEIGHT - y - 1;
    set_windows(x, y, width, height);
    write_cmd(MEMORY_WRITE);
    ST7735_set_pin(ST7735_DC_PIN, true);
    ST7735_set_pin(ST7735_CS_PIN, false);
    uint8_t buffer[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    for (uint8_t i = x; i < x + width; i++) {
        for (uint8_t j = y; j < y + height; j++) {
            spi_write_blocking(SPI_PORT, buffer, 2);
        }
    }
    ST7735_set_pin(ST7735_CS_PIN, true);
}

void ST7735::drawCircle(int16_t xc, int16_t yc, uint16_t r, uint16_t color) {
    int16_t x = -r;
    int16_t y = 0;
    int16_t e = 2 - (2 * r);
    do {
        drawPixel(xc + x, yc - y, color);
        drawPixel(xc - x, yc + y, color);
        drawPixel(xc + y, yc + x, color);
        drawPixel(xc - y, yc - x, color);
        int16_t _e = e;
        if (_e <= y)
            e += (++y * 2) + 1;
        if ((_e > x) || (e > y))
            e += (++x * 2) + 1;
    } while (x < 0);
}

void ST7735::drawFilledCircle(int16_t xc,
                              int16_t yc,
                              uint16_t r,
                              uint16_t color) {
    int16_t x = r;
    int16_t y = 0;
    int16_t e = 1 - x;
    while (x >= y) {
        drawLine(xc + x, yc + y, xc - x, yc + y, color);
        drawLine(xc + y, yc + x, xc - y, yc + x, color);
        drawLine(xc - x, yc - y, xc + x, yc - y, color);
        drawLine(xc - y, yc - x, xc + y, yc - x, color);
        ++y;
        if (e >= 0) {
            x--;
            e += 2 * ((y - x) + 1);
        } else
            e += (2 * y) + 1;
    }
}

void ST7735::setFont(const GFXfont* font) {
    myFont = font;
}

void ST7735::printChar(uint8_t x,
                       uint8_t y,
                       uint8_t character,
                       uint16_t color) {
    if (character < myFont->first || character > myFont->last)
        return;
    character -= myFont->first;
    GFXglyph* glyph = myFont->glyph + character;
    uint8_t* bitmap = myFont->bitmap;

    uint16_t bitmapOffset = glyph->bitmapOffset;
    uint8_t width = glyph->width, height = glyph->height;
    int8_t xOffset = glyph->xOffset;
    uint8_t yOffset = myFont->yAdvance + glyph->yOffset;
    uint8_t bits = 0, abit = 0;

    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            if (!(abit++ & 7)) {
                bits = bitmap[bitmapOffset++];
            }
            if (bits & 0x80) {
                drawPixel(x + xOffset + j, y + yOffset + i, color);
            }
            bits <<= 1;
        }
    }
}

void ST7735::print(uint8_t x, uint8_t y, uint8_t* string, uint16_t color) {
    for (uint8_t i = 0; string[i]; i++) {
        uint8_t character = string[i];
        GFXglyph* glyph = myFont->glyph + character - myFont->first;
        if (x + glyph->width + glyph->xOffset > WIDTH) {
            x = 0;
            y += myFont->yAdvance;
        }
        printChar(x, y, character, color);
        x += glyph->xAdvance;
    }
}

void ST7735::drawImage(uint8_t x,
                       uint8_t y,
                       uint8_t width,
                       uint8_t height,
                       const uint16_t* image) {
    if (x + width > WIDTH || y + height > HEIGHT)
        return;
    set_windows(x, y, width, height);
    write_cmd(MEMORY_WRITE);
    ST7735_set_pin(ST7735_DC_PIN, true);
    ST7735_set_pin(ST7735_CS_PIN, false);
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            uint8_t buffer[2] = {(uint8_t)(image[i * width + j] >> 8),
                                 (uint8_t)(image[i * width + j] & 0xFF)};
            spi_write_blocking(SPI_PORT, buffer, 2);
        }
    }
    ST7735_set_pin(ST7735_CS_PIN, true);
}
