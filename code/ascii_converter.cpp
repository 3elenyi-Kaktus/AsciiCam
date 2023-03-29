#include "ascii_converter.h"
#include "iostream"


std::vector<std::vector<u_char>> ConvertFrameToASCII(cv::Mat &frame) {
    std::unordered_map<u_char, char> characters = {{0, ' '},
                                                   {1, '.'},
                                                   {2, ':'},
                                                   {3, 'r'},
                                                   {4, '4'},
                                                   {5, 'W'},
                                                   {6, '@'}};
    size_t out_height = frame.rows;
    size_t out_width = frame.cols;
    std::vector<std::vector<u_char>> brightness_table(out_height, std::vector<u_char>(out_width));
    u_char min_br = UCHAR_MAX;
    u_char max_br = 0;
    for (size_t i = 0; i < out_height; ++i) {
        for (size_t j = 0; j < out_width; ++j) {
            brightness_table[i][j] = (uchar) *(frame.ptr(i) + j);
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