#include "main.h"

bool ssrc_checker(void *content, size_t size, size_t position) {
    
    mjr_frame_hdr *frame; 
    rtp_hdr *r;
    uint32_t constant_ssrc;
    int i;

    i = 0;
    while (position < size)
    {
        // grabe the frame header.
        frame = (mjr_frame_hdr*)content;

        // grab the RTP packet header immediately after it.
        r = (rtp_hdr *)(content + sizeof(mjr_frame_hdr));

        if (i == 0)
            constant_ssrc = r->ssrc_be;   
        else if (constant_ssrc != r->ssrc_be) {
            printf("ssrc drift: %u vs %u ", constant_ssrc, r->ssrc_be);
        }
        // ensure the first bytes of this frame immediately start with "MEET"
        if (memcmp(frame, "MEET", 4) != 0) 
        {
            printf("incorrect block");
            return false;
        }

        uint16_t len = ntohs(frame->len_be);

        if (position + sizeof(mjr_frame_hdr) + len > size)
            break;                             

        position += sizeof(mjr_frame_hdr) + len;
        content += sizeof(mjr_frame_hdr);
        i++;
        content += len;
    }
    printf("here is %u\n", constant_ssrc);
    return true;
}