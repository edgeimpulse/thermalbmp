
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "bitmap_helpers.h"

bool read_thremal_csv(const char* file_name, std::vector<float>& values, int offset)
{
    std::ifstream f(file_name);
    if (f.is_open()) {
        std::string line;
        if (std::getline(f,line)) { // skip the first line
            if (std::getline(f,line)) {
                std::stringstream ss(line);
                for (float v; ss >> v;) {
                    values[offset++] = (v);
                    if (ss.peek() == ',')
                        ss.ignore();
                }
                return true;
            }
        }
    }
    return false;
}

inline float map_linear(float x_min, float x_max, float x, float y_min, float y_max)
{
    if (x < x_min) return y_min;
    else if (x > x_max) return y_max;
    else {
        return y_min + (y_max - y_min) / (x_max - x_min) * (x - x_min);
    }
}

// map temperature (15,35) to greyscale (0,255)
void csv_to_8bit_greyscale(std::vector<float>& values, size_t size)
{
    const float min_temp = 15.0f;
    const float max_temp = 35.0f;
    for (size_t i = 0; i < size; i++) {
        values[i] = map_linear(min_temp, max_temp, values[i], 0.0f, 255.0f);
    }
}

void greyscale_to_rgb(std::vector<float>& values, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        uint32_t grey = static_cast<uint8_t>(values[i]);
        uint32_t rgb = (grey << 16) | (grey << 8) | (grey);
        values[i] = static_cast<float>(rgb);
    }    
}

int main(int argc, char* argv[])
{
    if (argc != 3) 
        return -1;

    const size_t w = 38;
    const size_t h = 38;
    const size_t size = w * h;
    std::vector<float> values(size);
    std::fill(values.begin(), values.end(), 0.0f);
    
    // read input csv
    const size_t csv_height = 17;
    const int offset = w * ((h - csv_height) / 2); // split the padding between top and bottom
    if (!read_thremal_csv(argv[1], values, offset - 1)) // -1 because first value is invalid
        return 1;

    // convert temperature values to greyscale
    csv_to_8bit_greyscale(values, size);

    // convert greyscale to RGB (EI internal format)
    greyscale_to_rgb(values, size);

    // save to bitmap
    create_bitmap_file(argv[2], values.data(), w, h);

    return 0;
}
