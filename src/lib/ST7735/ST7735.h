#ifndef _ST7735_H_
#define _ST7735_H_

#include "hardware/spi.h"
#include "pico/stdlib.h"

// Command Register Definition
#define SET_SOFT_REST 0x01
#define SET_SLEEP 0x10
#define SET_NORMAL_DISPLAY 0x13
#define SET_INVERSE 0x20
#define SET_DISPLAY 0x28
#define SET_COL_ADDRESS 0x2A
#define SET_ROW_ADDRESS 0x2B
#define MEMORY_WRITE 0x2C
#define MEMORY_READ 0x2E
#define SET_PARTICAL 0x12
#define SET_PARTICAL_ADDRESS 0x30
#define SET_TEARING 0x34
#define SET_MEMORY_ACCESS 0x36
#define SET_IDLE 0x38
#define SET_PIXEL_MODE 0x3A

struct GFXglyph {
    uint16_t bitmapOffset;  ///< Pointer into GFXfont->bitmap
    uint8_t width;          ///< Bitmap dimensions in pixels
    uint8_t height;         ///< Bitmap dimensions in pixels
    uint8_t xAdvance;       ///< Distance to advance cursor (x axis)
    int8_t xOffset;         ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
};

struct GFXfont {
    uint8_t* bitmap;   ///< Glyph bitmaps, concatenated
    GFXglyph* glyph;   ///< Glyph array
    uint8_t first;     ///< ASCII extents (first char)
    uint8_t last;      ///< ASCII extents (last char)
    uint8_t yAdvance;  ///< Newline distance (y axis)
};

uint16_t CRGB(uint32_t color);
uint16_t CRGB(uint8_t r, uint8_t g, uint8_t b);

class ST7735 {
   private:
    uint32_t FREQUENCY;
    spi_inst_t* SPI_PORT;

    // Pin Definition
    uint8_t ST7735_DC_PIN;
    uint8_t ST7735_CS_PIN;
    uint8_t ST7735_SDA_PIN;
    uint8_t ST7735_SCL_PIN;

    uint8_t WIDTH;
    uint8_t HEIGHT;
    const GFXfont* myFont;

    void swap(uint8_t* x1, uint8_t* x2);
    void ST7735_set_pin(uint8_t pin, bool status);
    void write_cmd(uint8_t cmd);
    void write_data(uint8_t data);
    void hardware_init();
    void ST7735_init();
    void drawPixel(uint8_t x, uint8_t y, uint16_t color);
    void set_windows(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

   public:
    ST7735(uint8_t scl,
           uint8_t sda,
           uint8_t cs,
           uint8_t dc,
           uint8_t width,
           uint8_t height,
           uint32_t freq,
           spi_inst_t* spi);
    ~ST7735();
    void clear();
    void fill(uint16_t color);
    void drawFastHLine(uint8_t x, uint8_t y, uint8_t width, uint16_t color);
    void drawFastVLine(uint8_t x, uint8_t y, uint8_t height, uint16_t color);
    void drawLine(uint8_t x1,
                  uint8_t y1,
                  uint8_t x2,
                  uint8_t y2,
                  uint16_t color);
    void drawRectangle(uint8_t x,
                       uint8_t y,
                       uint8_t width,
                       uint8_t height,
                       uint16_t color);
    void drawFilledRectangle(uint8_t x,
                             uint8_t y,
                             uint8_t width,
                             uint8_t height,
                             uint16_t color);
    void drawCircle(int16_t xc, int16_t yc, uint16_t r, uint16_t color);
    void drawFilledCircle(int16_t xc, int16_t yc, uint16_t r, uint16_t color);

    void setFont(const GFXfont* font);
    void printChar(uint8_t x, uint8_t y, uint8_t character, uint16_t color);
    void print(uint8_t x, uint8_t y, uint8_t* string, uint16_t color);
    void drawImage(uint8_t x,
                   uint8_t y,
                   uint8_t width,
                   uint8_t height,
                   const uint16_t* image);
};

#endif