#include <iostream>
#include <string>

int main(int argc, char** argv) {

    std::cout << "Number of arguments (argc): " << argc << "\n\n";

    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << "\n";
    }

    return 0;


    // if (argc < 2) {
    //     std::cout << "Usage: videoinfo <path/to/video> [--json]\n";
    //     return 0;
    // }

    // std::string path = argv[1];
    // bool json = (argc >= 3 && std::string(argv[2]) == "--json");

    // std::cout << "TODO: analize file: " << path << (json ? " (json mode)\n" : "\n");
    
    // return 0;
}