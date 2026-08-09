#include "main.h"


size_t frame_len(void *content, size_t size, size_t position)  {
    size_t block_count;
    mjr_frame_hdr *frame; 
    uint16_t len;
    
    block_count = 0;
    while (position < size)
    {
        // grabe the frame header.
        frame = (mjr_frame_hdr*)content;

        // ensure the first bytes of this frame immediately start with "MEET"
        if (memcmp(frame, "MEET", 4) != 0) 
        {
            printf("incorrect block");
            return 0;
        }

        len = ntohs(frame->len_be);

        if (position + sizeof(mjr_frame_hdr) + len > size)
            break;                             

        position += sizeof(mjr_frame_hdr) + len;
        content += sizeof(mjr_frame_hdr);
        content += len;
        block_count++;
    }

    return block_count;
}

uint16_t    *alloc_frame_array16_t(void *content ,size_t size, size_t position) {
    uint32_t frame_count;
    uint16_t *frame_array; 
    void *temp_content;

    temp_content = content;
    frame_count = frame_len(temp_content, size, position);
    frame_array = (uint16_t *)malloc(sizeof(uint16_t) * frame_count + 1);

    bzero((void*)frame_array, (frame_count * sizeof(uint16_t)));
    frame_array[frame_count] = -1;
    return frame_array;
}

uint32_t    *alloc_frame_array32_t(void *content ,size_t size, size_t position) {
    uint32_t frame_count;
    uint32_t *frame_array; 
    void *temp_content;

    temp_content = content;
    frame_count = frame_len(temp_content, size, position);
    frame_array = (uint32_t *)malloc(sizeof(uint32_t) * frame_count + 1);

    bzero((void*)frame_array, (frame_count * sizeof(uint32_t)));
    frame_array[frame_count] = -1;
    return frame_array;
}



t_drift *drift_detection(void *content, size_t size, size_t position) {
    mjr_frame_hdr *frame; 
    rtp_hdr *r;
    uint32_t *drift;
    uint16_t *sequence;
    t_drift *drift_container;
    
    int clock_rate;

    void *traverse;

    uint32_t first_recv_be = 0;
    uint32_t first_ts = 0;
    uint16_t len = 0;


    traverse = content;
    drift = alloc_frame_array32_t(content, size, position);
    sequence = alloc_frame_array16_t(content, size, position);

    int i = 0;
    while (position < size)
    {
        // grabe the frame header.
        frame = (mjr_frame_hdr*)traverse;

        // grab the RTP packet header immediately after it.
        r = (rtp_hdr *)(traverse + sizeof(mjr_frame_hdr));
        if (is_video(r) == IS_VIDEO) {
            clock_rate = 90000.0;
        } else {
            clock_rate = 48000.0;
        }
        // ensure the first bytes of this frame immediately start with "MEET"
        if (memcmp(frame, "MEET", 4) != 0) 
        {
            printf("incorrect block");
            return NULL;
        }

        if (i == 1)
        {
            first_recv_be = ntohl(frame->recv_be);  
            first_ts = ntohl(r->ts_be);
        }

        if (i > 0)
        {
            drift[i - 1] = (r->ts_be - first_ts) / clock_rate - (first_recv_be - frame->recv_be) / 1000.0;
            sequence[i - 1] = ntohs(r->seq_be);
        }

        len = ntohs(frame->len_be);

        if (position + sizeof(mjr_frame_hdr) + len > size)
            break;                             

        position += sizeof(mjr_frame_hdr) + len;
        traverse += sizeof(mjr_frame_hdr);
        i++;
        traverse += len;
    }

    drift_container =(t_drift *)malloc(sizeof(t_drift));
    drift_container->drift_timers = drift;
    drift_container->sequences = sequence;
    return drift_container;
}
