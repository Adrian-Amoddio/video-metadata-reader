#include <iostream>
#include <iomanip>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: videoinfo <path/to/video>\n";
        return 0;
    }
    std::string path = argv[1];

    std::cout<< "Filepath: " << path << "\n";
    return 0;
    
}



