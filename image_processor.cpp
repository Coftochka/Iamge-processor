#include "filters.h"
#include <cstring>

const size_t DEFAULT_BUFFERSIZE = 1000;

bool Compare(std::string inp, std::string out, size_t buffersize = DEFAULT_BUFFERSIZE) {
    std::ifstream a(inp, std::ios::binary);
    std::ifstream b(out, std::ios::binary);
    char buffera[buffersize];
    char bufferb[buffersize];
    memset(buffera, 0, sizeof(buffera));
    memset(bufferb, 0, sizeof(bufferb));
    a.read(buffera, sizeof(buffera));  // NOLINT
    b.read(bufferb, sizeof(bufferb));  // NOLINT
    a.close();
    b.close();
    bool flag = true;
    for (int i = 0; i < sizeof(buffera); ++i) {
        if (buffera[i] != bufferb[i]) {
            std::cout << "AAAAAAAAAAAAAAAA Diff " << i << ' ' << static_cast<int>(static_cast<u_int8_t>(buffera[i]))
                      << ' ' << static_cast<int>(static_cast<u_int8_t>(bufferb[i])) << std::endl;
            flag = false;
        }
    }
    return flag;
}

int main(int argc, char** argv) {
    for (size_t i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == "--help") {
            std::ifstream man_file("man.txt");
            std::string line;
            while (std::getline(man_file, line)) {
                std::cout << line << std::endl;
            }
            man_file.close();
            return 0;
        }
    }

    if (argc < 3) {
        std::cout << "Too few arguments. At least 2: input_file.bmp  output_file.bmp" << std::endl;
        return 0;
    }

    std::string input(argv[1]);
    std::string output(argv[2]);

    Image image(input);
    for (size_t i = 3; i < argc; ++i) {
        std::string current(argv[i]);
        if (current == "-crop") {
            if (i + 2 >= argc) {
                std::cout << "Too few arguments for crop" << std::endl;
                return 0;
            }
            std::string width(argv[++i]);
            std::string height(argv[++i]);
            try {
                Crop crop(std::stoi(height), std::stoi(width));
                crop.Process(image);
            } catch (...) {
                std::cout << "Invalid argumants for -crop" << std::endl;
                return 0;
            }
        } else if (current == "-gs") {
            GrayScale gs;
            gs.Process(image);
        } else if (current == "-neg") {
            Inversion inv;
            inv.Process(image);
        } else if (current == "-sharp") {
            Sharpening sharp;
            sharp.Process(image);
        } else if (current == "-edge") {
            if (i + 1 >= argc) {
                std::cout << "No threshold for edge detection" << std::endl;
                return 0;
            }
            std::string threshold(argv[++i]);
            try {
                EdgeDetection edge(std::stof(threshold));
                edge.Process(image);
            } catch (...) {
                std::cout << "Invalid argument for edge detection : " << threshold << std::endl;
                return 0;
            }
        } else {
            std::cout << "Invalid argument: " << current << std::endl;
        }
    }
    image.Save(output);
}
