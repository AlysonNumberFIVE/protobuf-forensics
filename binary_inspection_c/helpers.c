#include "main.h"

void print_seq(uint16_t seq_be) { 
    printf("  seq=%u", ntohs(seq_be)); 
}

void print_ts(uint32_t ts_be) { 
    printf("  ts=%u ", ntohl(ts_be)); 
}

void print_ssrc(uint32_t ssrc_be) { 
    printf("  ssrc=0x%08x", ntohl(ssrc_be)); 
}

void    print_recv(uint32_t recv_be)
{
    uint32_t recv = ntohl(recv_be);
    printf("  recv time = %u ms ", recv);
}

void    print_len(uint16_t len_be)
{
    uint16_t len = ntohs(len_be);
    printf("  payload length = %u bytes ", len);
}

void    print_frame_array(void *content, size_t size, size_t position, t_drift *drift_list) {
    uint32_t frame_count;
    void *temp_content;
    uint32_t i;
    int jump_counter;

    temp_content = content;
    frame_count = frame_len(temp_content, size, position);

    printf("=== timestamp of timestamp drifts ===\n");
    i = 0;
    jump_counter = 0;
    while (i < frame_count)
    {
        if (drift_list->drift_timers[i] != 0)
        {
            printf("[%ld %u] ", drift_list->sequences[i] + 1785394762873,drift_list->drift_timers[i]);
            jump_counter++;
        }
        i++;
    }
    printf("\n==== total jumps: %d ====\n", jump_counter);
}

int is_video(rtp_hdr *r) {
    uint8_t pt = r->b1 & 0x7f;  
    return pt;
}