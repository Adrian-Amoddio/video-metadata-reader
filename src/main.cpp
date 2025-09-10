
#include <string>

#include <iostream>
extern "C" {
#include <libavutil/avutil.h>
}
int main() {
    std::cout << "FFmpeg version: " << av_version_info() << "\n";
    return 0;
}


    // if (argc < 2) {
    //     std::cout << "Usage: videoinfo <path/to/video> [--json]\n";
    //     return 0;
    // }

    // std::string path = argv[1];
    // bool json = (argc >= 3 && std::string(argv[2]) == "--json");

    // std::cout << "TODO: analize file: " << path << (json ? " (json mode)\n" : "\n");
    
    // return 0;
