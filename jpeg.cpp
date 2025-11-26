#include <unistd.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void PrintHelp()
{
    std::cout << 
R"(Usage: jpeg [OPTION]... [SOURCE] [DEST]

Arguments:
    [SOURCE] Path to a file
    [DEST] Path to file output

Options:
    -h - print this message)" << std::endl;
}


void jpeg(unsigned char** img)
{
}

int main(int argc, char* argv[])
{
    if (argc <= 2)
    {
        PrintHelp();
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
                exit(1);
        }
    }


    int width, height, channels = 3;
    unsigned char *img = stbi_load(argv[1], &width, &height, nullptr, channels);
    if (img == NULL)
    {
        std::cout << "Error in loading the image" << std::endl;
        return 1;
    }
    std::cout << "Loaded image with width of " << width
              << ", height of " << height << std::endl;

    jpeg(&img);

    stbi_write_png(argv[2], width, height, channels, img, width * channels);
    std::cout << "Image exported" << std::endl;
    return 0;
}
