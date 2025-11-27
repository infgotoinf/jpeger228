#include <unistd.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void PrintHelp()
{
    std::cout << 
R"(Usage: jpeg [OPTION]... [SOURCE]

Arguments:
    [SOURCE] Path to an image

Options:
    -h - print this message)" << std::endl;
}


struct RGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

struct fRGB
{
    float R;
    float G;
    float B;

    fRGB(fRGB const &) = default;
};

static float Min(float a, float b)
{
    return a <= b ? a : b;
}

static float Max(float a, float b)
{
    return a >= b ? a : b;
}

fRGB rgbToFrgb (RGB rgb)
{
    struct fRGB frgb = { .R = (float)rgb.R / 255
                       , .G = (float)rgb.G / 255
                       , .B = (float)rgb.B / 255 };
    return frgb;
}
    

void exportImage(const char* name, int width, int height, int channels, unsigned char* img)
{
    stbi_write_png(name, width, height, channels, img, width * channels);
    std::cout << "Image exported to " << name << std::endl;
}


// instead RGB, JPG images use YCbCr
// in each function I convert RBG to YCbCr and to RBG again
// Y - lumiunance
void toLuminance(unsigned char* img, int width, int height, int channels)
{
    for (size_t i = 0; i < width * height * channels; i += channels)
    {
        struct RGB rgb = { .R = img[i], .G = img[i+1], .B = img[i+2] };
        struct fRGB frgb = rgbToFrgb(rgb); 

        float Y = (float)(0.2989 * frgb.R + 0.5866 * frgb.G + 0.1145 * frgb.B);

        img[i]    = Max(0.0f, Min(1.0f, (float)(Y))) * 255;
        img[i+1]  = Max(0.0f, Min(1.0f, (float)(Y))) * 255;
        img[i+2]  = Max(0.0f, Min(1.0f, (float)(Y))) * 255;
    }
}

// Cb - chominance blue
void toCrominanceBlue(unsigned char* img, int width, int height, int channels)
{
    for (size_t i = 0; i < width * height * channels; i += channels)
    {
        struct RGB rgb = { .R = img[i], .G = img[i+1], .B = img[i+2] };
        struct fRGB frgb = rgbToFrgb(rgb); 

        float Cb = (float)(-0.1687 * frgb.R - 0.3313 * frgb.G + 0.5000 * frgb.B);

        img[i]   = Max(0.0f, Min(1.0f, (float)( 0.0000 * Cb))) * 255;
        img[i+1] = Max(0.0f, Min(1.0f, (float)(-0.3456 * Cb))) * 255;
        img[i+2] = Max(0.0f, Min(1.0f, (float)( 1.7710 * Cb))) * 255;
    }
}

// Cr - chrominance red
void toChrominanceRed(unsigned char* img, int width, int height, int channels)
{
    for (size_t i = 0; i < width * height * channels; i += channels)
    {
        struct RGB rgb = { .R = img[i], .G = img[i+1], .B = img[i+2] };
        struct fRGB frgb = rgbToFrgb(rgb); 

        float Cr = (float)(0.5000 * frgb.R - 0.4184 * frgb.G - 0.0816 * frgb.B);

        img[i]   = Max(0.0f, Min(1.0f, (float)( 1.4022 * Cr))) * 255;
        img[i+1] = Max(0.0f, Min(1.0f, (float)(-0.7145 * Cr))) * 255;
        img[i+2] = Max(0.0f, Min(1.0f, (float)( 0.0000 * Cr))) * 255;
    }
}


int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        PrintHelp();
        return 1;
    }

    char opt;
    while ((opt = getopt(argc, argv, ":h")) != -1)
    {
        switch (opt)
        {
            case 'h':
                PrintHelp();
                return 0;
            case '?':
                PrintHelp();
                return 1;
        }
    }


    int width, height, channels = 3;
    unsigned char* img = stbi_load(argv[1], &width, &height, nullptr, channels);
    if (img == NULL)
    {
        std::cout << "Error in loading the image" << std::endl;
        return 1;
    }
    std::cout << "Loaded image with width of " << width
              << ", height of " << height << '\n' << std::endl;

    toLuminance(img, width, height, channels);
    exportImage("export/Luminance.png", width, height, channels, img);
    
    img = stbi_load(argv[1], &width, &height, nullptr, channels);
    toCrominanceBlue(img, width, height, channels);
    exportImage("export/ChominanceBlue.png", width, height, channels, img);
    
    img = stbi_load(argv[1], &width, &height, nullptr, channels);
    toChrominanceRed(img, width, height, channels);
    exportImage("export/ChrominanceRed.png", width, height, channels, img);

    return 0;
}
