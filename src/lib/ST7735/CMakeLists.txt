add_library(ST7735
    ST7735.h
    ST7735.cpp
)

target_link_libraries(ST7735
    pico_stdlib
    hardware_spi
)
