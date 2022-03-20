extern "C" {
    #include <libavutil/imgutils.h>
    #include <libavcodec/avcodec.h>
    #include <libavcodec/packet.h>
    #include <libavutil/avutil.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include <iostream>
#include <cstdint>
#include <stddef.h>

#define INBUF_SIZE 4096

typedef unsigned short uint16_t;

#ifndef attribute_align_arg
#if ARCH_X86_32 && AV_GCC_VERSION_AT_LEAST(4,2)
#    define attribute_align_arg __attribute__((force_align_arg_pointer))
#else
#    define attribute_align_arg
#endif
#endif

static int decode_packet(AVCodecContext* dec, const AVPacket *pkt, AVFrame* frame, uint8_t** video_dst_data, int* video_dst_linesize, const int width, const int height, enum AVPixelFormat pix_fmt, struct SwsContext *pSwsCtx, FILE* dest, uint16_t, uint16_t);
void dumpFrame(uint8_t** video_dst_data, int* video_dst_linesize, int width, int height, FILE* f);
static double get_fps(AVCodecContext* avctx);

static int video_dst_bufsize;
static int num_img = 0;

int main(int argc, const char** argv)
{
    double fps = 0.0;
    float mspf = 1.0;
    const uint16_t fmt_id = 0xDEAD;
    const uint16_t ver_id = 0x0000;
    static uint8_t* rgbBuffer = NULL;
    static int numBytes;
    static struct SwsContext *pSwsCtx = NULL;
    static uint8_t *video_dst_data[4] = {NULL};
    static int      video_dst_linesize[4];
    static uint16_t width, height;
    static uint16_t dst_width = 240, dst_height = 160;
    static uint32_t num_img_ptr = 0;
    static enum AVPixelFormat pix_fmt;
    FILE *f = NULL;
    int ret = EXIT_SUCCESS;
    static AVFormatContext *fmt_ctx = NULL;
    uint8_t *data;
    size_t   data_size;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    static AVCodecContext *video_dec_ctx = NULL;
    AVCodecContext *c= NULL;
    static FILE *video_dst_file = NULL;
    const char* filename, *outfilename;
    static AVFrame *frame = NULL;
    static AVFrame *frameRGB = NULL;
    AVPacket *pkt = NULL;
    AVCodecParserContext *parser = NULL;
    const AVCodec *codec;
    static int video_stream_idx = -1;
    int status = -1;
    enum AVMediaType type = AVMEDIA_TYPE_VIDEO;
    AVStream* st = NULL;

    if(argc < 3) {
        std::cerr << "Usage: " << argv[0] << " /path/to/input"  << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }
    
    filename = argv[1];
    outfilename     = argv[2];

    if(avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0) {
        std::cerr << "Coult not open source file " << filename << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }

    // retrieve stream information
    if(avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        std::cerr << "Coult not find stream information" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }

    // find video stream
    status = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if(status < 0) {
        std::cerr << "Could not find " << av_get_media_type_string(type) << " stream in input" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }
    video_stream_idx = status;
    st = fmt_ctx->streams[video_stream_idx];
    // dump input information
    av_dump_format(fmt_ctx, 0, filename, 0);
    if(fmt_ctx == NULL) {
        ret = EXIT_FAILURE;
        goto end;
    }
    std::cout << st->codecpar->codec_id << std::endl;
    codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec) {
        std::cerr <<  "Codec not found" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }
    std::cout << codec->name << std::endl;
    // parser = av_parser_init(codec->id);

    // if (!parser) {
    //     std::cerr << "Parser not found" << std::endl;
    //     ret = EXIT_FAILURE;
    //     goto end;
    // }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        std::cerr << "Could not allocate video codec context" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }


    if(avcodec_parameters_to_context(c, st->codecpar) < 0) {
        std::cerr << "Failed to open video codec" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }

    fps = av_q2d(st->r_frame_rate);
    
    std::cout << "Resolution:" << c->width << "x" << c->height << std::endl;
    std::cout << "FPS:" << fps << std::endl;
    
    mspf = 1000.0 / fps;

    width = c->width;
    height = c->height;
    pix_fmt = c->pix_fmt;
    ret = av_image_alloc(video_dst_data, video_dst_linesize,
                         dst_width, dst_height, AV_PIX_FMT_BGRA, 1);
    if (ret < 0) {
        std::cerr << "Could not allocate raw video buffer\n" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }
    video_dst_bufsize = ret;

   /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */
 
    /* Init the decoders */
    if (avcodec_open2(c, codec, NULL) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        exit(1);
    }
 
    // f = fopen(filename, "rb");
    // if (!f) {
    //     fprintf(stderr, "Could not open %s\n", filename);
    //     exit(1);
    // }

    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Coult not allocate video frame" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }

    frameRGB = av_frame_alloc();
    if (!frameRGB) {
        std::cerr << "Coult not allocate picture frame" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }
    
    pkt = av_packet_alloc();
    if(!pkt) {
        std::cerr << "Coult not allocate packet" << std::endl;
        ret = EXIT_FAILURE;
        goto end;
    }

    f = fopen(outfilename, "wb");
    num_img = 0;

    fwrite(&fmt_id, sizeof(uint16_t), 1, f);
    fwrite(&ver_id, sizeof(uint16_t), 1, f);
    fwrite(&dst_width, sizeof(uint16_t), 1, f);
    fwrite(&dst_height, sizeof(uint16_t), 1, f);
    num_img_ptr = ftell(f);
    fwrite(&num_img, sizeof(uint32_t), 1, f);
    fwrite(&mspf, sizeof(uint32_t), 1, f); // ms per frame

    

    // get SwsContext
    pSwsCtx = sws_getContext(
        width, height, pix_fmt,       // src Width Height pixFmt
        dst_width, dst_height, AV_PIX_FMT_BGRA, // dest Width Height pixFmt
        SWS_BICUBIC, 
        NULL,    // srcFilter
        NULL,    // destFiletr
        NULL     // param
    );

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx) {
            ret = decode_packet(c, pkt, frame, video_dst_data, video_dst_linesize, width, height, pix_fmt, pSwsCtx, f, dst_width, dst_height);
            av_packet_unref(pkt);
        }
        if (ret < 0)
            break;
    }

    if (video_dec_ctx)
        decode_packet(c, NULL, frame, NULL, NULL, width, height, pix_fmt, pSwsCtx, f, dst_width, dst_height);

    fseek(f, num_img_ptr, SEEK_SET);
    std::cout << "# of Frame:" <<  num_img << std::endl;
    fwrite(&num_img, sizeof(uint32_t), 1, f);
    fseek(f, 0, SEEK_END);
    
    fclose(f);
    

    
end:
    if(f != NULL) {
        fclose(f);
    }
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    if (video_dst_file)
        fclose(video_dst_file);
    av_frame_free(&frame);
    av_frame_free(&frameRGB);
    av_free(video_dst_data[0]);
    sws_freeContext(pSwsCtx);
    return EXIT_SUCCESS;
}

static int decode_packet(AVCodecContext* dec, const AVPacket *pkt, AVFrame* frame, uint8_t** video_dst_data, int* video_dst_linesize, const int width, const int height, enum AVPixelFormat pix_fmt, struct SwsContext *pSwsCtx, FILE* dest, uint16_t dest_width, uint16_t dest_height) {
    int ret = 0;
    FILE* f = NULL;
    static char buffer[512];
    ret = avcodec_send_packet(dec, pkt);
    if( ret < 0 ) {
        std::cerr << "Error submitting a packet for decodeing " << av_err2str(ret) << std::endl;
        return ret;
    }
    
    while(ret >= 0) {
        ret = avcodec_receive_frame(dec, frame);
        if(ret < 0) {
            if(ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                return 0;
            }
            std::cerr << "Error during decodding " << av_err2str(ret) << std::endl;
            return ret;
        }
        if(dec->codec->type == AVMEDIA_TYPE_VIDEO)
        {
            // output video frame
            if(video_dst_data && video_dst_linesize) {
                sws_scale(pSwsCtx, frame->data, frame->linesize, 0, height, video_dst_data, video_dst_linesize);
                dumpFrame(video_dst_data, video_dst_linesize, dest_width, dest_height, dest);
                num_img++;                
            }
        }
        av_frame_unref(frame);
        if(ret < 0)
            return ret;
    }
    return 0;
}

void dumpFrame(uint8_t** video_dst_data, int* video_dst_linesize, int width, int height, FILE* f) {
    for(int y=0; y<height; y++) {
        fwrite(video_dst_data[0]+y*video_dst_linesize[0], 1, width*4, f);
    }
    
}
