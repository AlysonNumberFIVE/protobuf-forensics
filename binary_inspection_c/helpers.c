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

static int host_is_little_endian(void) {
    uint16_t probe = 0x0001;
    return *(uint8_t *)&probe == 0x01;   /* is byte-0 the '1'? then low byte first */
}

void rtp_unpack_first16(uint16_t raw) {
    uint16_t v;
    if (host_is_little_endian())
        v = (uint16_t)((raw >> 8) | (raw << 8));   /* swap back to wire order */
    else
        v = raw;

    uint8_t version = (v >> 14) & 0x03;   /* V  : 2 bits */
    uint8_t padding = (v >> 13) & 0x01;   /* P  : 1 bit  */
    uint8_t ext     = (v >> 12) & 0x01;   /* X  : 1 bit  */
    uint8_t cc      = (v >>  8) & 0x0F;   /* CC : 4 bits */
    uint8_t marker  = (v >>  7) & 0x01;   /* M  : 1 bit  */
    uint8_t pt      =  v        & 0x7F;   /* PT : 7 bits */

    printf("  version=%u padding=%u ext=%u cc=%u marker=%u pt=%u\n",
           version, padding, ext, cc, marker, pt);
}