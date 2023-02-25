#include "vector"
#include "unordered_map"
#include "climits"
#include "cmath"

typedef unsigned char u_char;

enum {
    ASCII_SYMBOL_HEIGHT = 15,
    ASCII_SYMBOL_WIDTH = 9
};

u_char GetPixelsBrightness(std::vector<std::vector<u_char>> &image, size_t height_pos, size_t width_pos) {
    double total_brightness = 0;
    for (size_t i = height_pos; i < height_pos + ASCII_SYMBOL_HEIGHT; ++i) {
        for (size_t j = width_pos; j < width_pos + ASCII_SYMBOL_WIDTH; ++j) {
            total_brightness += image[i][j];
        }
    }
    int average_brightness = round(total_brightness / (ASCII_SYMBOL_HEIGHT * ASCII_SYMBOL_WIDTH));
    return average_brightness;
}


// Assuming that image is (height * 15) x (width * 9), with integer constants
std::vector<std::vector<u_char>> ConvertImageToASCII(std::vector<std::vector<u_char>> &image) {
    std::unordered_map<u_char, char> characters = {{0, ' '},
                                                   {1, '.'},
                                                   {2, ':'},
                                                   {3, 'r'},
                                                   {4, '4'},
                                                   {5, 'W'},
                                                   {6, '@'}};
    size_t out_height = image.size() / ASCII_SYMBOL_HEIGHT;
    size_t out_width = image[0].size() / ASCII_SYMBOL_WIDTH;
    std::vector<std::vector<u_char>> brightness_table(out_height, std::vector<u_char>(out_width));
    u_char min_br = UCHAR_MAX;
    u_char max_br = 0;
    for (size_t i = 0; i < out_height; ++i) {
        for (size_t j = 0; j < out_width; ++j) {
            brightness_table[i][j] = GetPixelsBrightness(image, i * ASCII_SYMBOL_HEIGHT, j * ASCII_SYMBOL_WIDTH);
            min_br = std::min(min_br, brightness_table[i][j]);
            max_br = std::max(max_br, brightness_table[i][j]);
        }
    }
    double range =
            (max_br - min_br) / static_cast<double>(characters.size() - 1); // To determine constraints of mapping
    for (size_t i = 0; i < out_height; ++i) {
        for (size_t j = 0; j < out_width; ++j) {
            u_char br_level = round(
                    (brightness_table[i][j] - min_br) / range); // Maybe needs some debugging for constraints
            brightness_table[i][j] = characters[br_level];
        }
    }
    return brightness_table; // Out matrix. Probably is better to replace it with strings array.
}