#include "main.h"

// inspect_video_stream ensures that the packets on a RPT video file format arrives in tact
void inspect_video_stream(void *content, size_t size, size_t position) {
    mjr_frame_hdr *frame;
    rtp_hdr *r;
    int i;
    uint32_t prev_ts;
    uint8_t  prev_m;

    void *tmp_content = content;

    i = 0;
    while (position < size)
    {
        // grabe the frame header.
        frame = (mjr_frame_hdr*)tmp_content;

        // grab the RTP packet header immediately after it.
        r = (rtp_hdr *)(tmp_content + sizeof(mjr_frame_hdr));
       
        if (is_video(r) == IS_AUDIO)
            return ;

        // ensure the first bytes of this frame immediately start with "MEET"
        if (memcmp(frame, "MEET", 4) != 0)
        {
            printf("incorrect block");
            return;
        }

        uint32_t ts = ntohl(r->ts_be);
        uint8_t  m  = (r->b1 & 0x80) >> 7;

        if (ts != prev_ts && prev_m == 0) {
            // timestamp advanced to a new frame, but the previous frame
            // never got its marker == incomplete frame
            printf("** video frame incomplete before ts %u (packet %d, prev m=0)\n", ts, i);
        }
        prev_ts = ts;
        prev_m  = m;

        uint16_t len = ntohs(frame->len_be);

        if (position + sizeof(mjr_frame_hdr) + len > size)
            break;                             

        position += sizeof(mjr_frame_hdr) + len;
        tmp_content += sizeof(mjr_frame_hdr);
        i++;
        tmp_content += len;

    }

}