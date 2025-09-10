#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>


extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>     
#include <libavutil/avutil.h>
#include <libavutil/rational.h>     
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>


}

// Converts the FFmpeg error code to a string and returns
static std::string av_err(int errornum) {
    char buffer[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errornum, buffer, sizeof(buffer));
    return std::string(buffer);
}



int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: videoinfo <path/to/video>\n";
        return 0;
    }
    std::string path = argv[1];

   
    


    av_log_set_level(AV_LOG_ERROR);  //less ffmpeg logs (unessicary stuff)

    AVFormatContext* video_metadata = nullptr; //This pointer doesn't point to anything yet. Placeholder for data that ffmpeg will load about the video file


    // Checks if stream info can be found in file
    int return_code = avformat_open_input(&video_metadata, argv[1], nullptr, nullptr);
    if (return_code < 0) {
        std::cerr << "open_input failed: " << av_err(return_code) << "\n"; //calls error message function to output a readable error. cerr is like cout but for errors.
        return 2;
    }


    // Gathers all info regarding video and audio streams
    return_code = avformat_find_stream_info(video_metadata, nullptr);
    if (return_code < 0) {
        std::cerr << "find_stream_info failed: " << av_err(return_code) << "\n";
        avformat_close_input(&video_metadata);
        return 3;
    }

    
    std::cout<< "Filepath: " << path << "\n";
    
    if (video_metadata->iformat && video_metadata->iformat->name)
        std::cout << "Container: " << video_metadata->iformat->name << "\n";




    // ---- Get bitrate info
    long long overall_bps = 0;

    long long duration_ms = -1;
    if (video_metadata->duration > 0) {
        duration_ms = video_metadata->duration * 1000LL / AV_TIME_BASE;
        std::cout << "Duration (ms): " << duration_ms << "\n";
    }


    if (video_metadata->bit_rate > 0) {
        overall_bps = video_metadata->bit_rate;
    } else if (duration_ms > 0) {
        // Try FFmpegs IO size
        long long size_bytes = -1;
        if (video_metadata->pb) {
            int64_t sz = avio_size(video_metadata->pb);
            if (sz >= 0) size_bytes = static_cast<long long>(sz);
        }
        // Fallback to OS file size if needed
        if (size_bytes < 0) {
            try {
                size_bytes = static_cast<long long>(std::filesystem::file_size(path));
            } catch (...) {
                size_bytes = -1;
            }
        }
        if (size_bytes > 0) {
            // bits per second = (bytes * 8) / seconds
            overall_bps = static_cast<long long>((size_bytes * 8.0) / (duration_ms / 1000.0));
        }
    }

    if (overall_bps > 0) {
        std::cout << "Overall bitrate (bps): " << overall_bps << "\n";
    } else {
        std::cout << "Overall bitrate (bps): unknown\n";
    }


    // Number of streams and types
    int total = static_cast<int>(video_metadata->nb_streams);
    int v = 0, a = 0, sub = 0, other = 0;

    for (unsigned i = 0; i < video_metadata->nb_streams; ++i) {
        const AVStream* st = video_metadata->streams[i];
        const AVCodecParameters* par = st->codecpar;
        switch (par->codec_type) {
            case AVMEDIA_TYPE_VIDEO:    ++v; break;
            case AVMEDIA_TYPE_AUDIO:    ++a; break;
            case AVMEDIA_TYPE_SUBTITLE: ++sub; break;
            default:                    ++other; break;
        }
    }

    std::cout << "Streams: total=" << total
            << "  video=" << v
            << "  audio=" << a
            << "  subtitle=" << sub
            << "  other=" << other << "\n";



    // Video stream details (resolution and fps)
    for (unsigned i = 0; i < video_metadata->nb_streams; ++i) {
        const AVStream* st = video_metadata->streams[i];
        const AVCodecParameters* par = st->codecpar;
        if (par->codec_type != AVMEDIA_TYPE_VIDEO) continue;

        const char* codec = avcodec_get_name(par->codec_id);
        int w = par->width, h = par->height;

        // FPS: prefer avg_frame_rate, fallback r_frame_rate
        double fps = 0.0;
        if (st->avg_frame_rate.num > 0 && st->avg_frame_rate.den > 0) {
            fps = av_q2d(st->avg_frame_rate);
        } else if (st->r_frame_rate.num > 0 && st->r_frame_rate.den > 0) {
            fps = av_q2d(st->r_frame_rate);
        }

        std::cout << "[Video #" << i << "] "
                << (codec ? codec : "unknown");
        if (w > 0 && h > 0) std::cout << "  " << w << "x" << h;
        if (fps > 0.0)     std::cout << "  " << std::fixed << std::setprecision(2) << fps << " fps";
        std::cout << "\n";
    }



    // Audio stream details
    for (unsigned i = 0; i < video_metadata->nb_streams; ++i) {
        const AVStream* st = video_metadata->streams[i];
        const AVCodecParameters* par = st->codecpar;
        if (par->codec_type != AVMEDIA_TYPE_AUDIO) continue;

        const char* codec = avcodec_get_name(par->codec_id);
        int channels = par->ch_layout.nb_channels;       
        int sample_rate = par->sample_rate;
        const char* sfmt = av_get_sample_fmt_name((AVSampleFormat)par->format);

        std::cout << "[Audio #" << i << "] "
                << (codec ? codec : "unknown");
        if (channels > 0)    std::cout << "  " << channels << "ch";
        if (sample_rate > 0) std::cout << "  " << sample_rate << " Hz";
        if (sfmt)            std::cout << "  " << sfmt;
        std::cout << "\n";
    }



    // Best streams summary
    int best_v = av_find_best_stream(video_metadata, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (best_v >= 0) {
        const AVStream* st = video_metadata->streams[best_v];
        const AVCodecParameters* par = st->codecpar;
        const char* codec = avcodec_get_name(par->codec_id);
        int w = par->width, h = par->height;
        double fps = 0.0;
        if (st->avg_frame_rate.num > 0 && st->avg_frame_rate.den > 0) fps = av_q2d(st->avg_frame_rate);
        else if (st->r_frame_rate.num > 0 && st->r_frame_rate.den > 0) fps = av_q2d(st->r_frame_rate);

        std::cout << "Best video: #" << best_v << "  "
                << (codec ? codec : "unknown");
        if (w > 0 && h > 0) std::cout << "  " << w << "x" << h;
        if (fps > 0.0)     std::cout << "  " << std::fixed << std::setprecision(2) << "  " << fps << " fps";
        std::cout << "\n";
    }

    int best_a = av_find_best_stream(video_metadata, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (best_a >= 0) {
        const AVStream* st = video_metadata->streams[best_a];
        const AVCodecParameters* par = st->codecpar;
        const char* codec = avcodec_get_name(par->codec_id);
        int channels = par->ch_layout.nb_channels;
        int sample_rate = par->sample_rate;

        std::cout << "Best audio: #" << best_a << "  "
                << (codec ? codec : "unknown");
        if (channels > 0)    std::cout << "  " << channels << "ch";
        if (sample_rate > 0) std::cout << "  " << sample_rate << " Hz";
        std::cout << "\n";
    }




    //------------------------------------------------------------------------------//

    // This frees memory and sets video_metadata back to nullptr.
    avformat_close_input(&video_metadata);
    return 0;



















    
}



