#include <iostream>
#include <iomanip>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

// Converts the FFmpeg error code to a string and returns
static std::string av_err(int errornum) {
    char buffer[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, buffer, sizeof(buffer));
    return std::string(buffer);
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



