#pragma once

#include <cstdint>

struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a = 255;
};

struct SOBELIMG {
    float r;
    float g;
    float b;
    std::uint8_t a = 255;
};
