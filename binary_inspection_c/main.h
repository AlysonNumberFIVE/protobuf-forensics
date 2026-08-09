#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define MJR_MAGIC     "MJR00002"   /* first 8 bytes, memcmp (no NUL terminator) */
#define MJR_FRAME_TAG "MEET"       /* marker at the start of every frame        */

#define IS_AUDIO 111
#define IS_VIDEO 107

#pragma pack(push, 1)
typedef struct {
    char     marker[4];   // "MEET"  (note: 4 bytes here, not 8 like the file magic)
    uint32_t recv_be;     // receive time, ms — big-endian, ntohl to use
    uint16_t len_be;      // payload length in bytes — big-endian, ntohs to use
} mjr_frame_hdr;          // sizeof == 10 thanks to pack(1)
#pragma pack(pop)



#pragma pack(push, 1)
typedef struct {
    uint8_t  b0;        /* V(2) P(1) X(1) CC(4) */
    uint8_t  b1;        /* M(1) PT(7)           */
    uint16_t seq_be;    /* ntohs */
    uint32_t ts_be;     /* ntohl */
    uint32_t ssrc_be;   /* ntohl */
    /* then CC * uint32_t CSRC entries (CC == b0 & 0x0F; it's 0 in your files) */
} rtp_hdr;
#pragma pack(pop)


typedef struct file_content {
    void    *content; 
    size_t  size;
}   t_file_content;

typedef struct drift_structure {
    uint32_t *drift_timers;
    uint16_t *sequences;
}   t_drift;


// drift.c

size_t  frame_len(void *content, size_t size, size_t position);
t_drift *drift_detection(void *content, size_t size, size_t position);


// helpers.c

void    print_frame_array(void *content, size_t size, size_t position, t_drift *drift_list);
void    print_seq(uint16_t seq_be);
void    print_ts(uint32_t ts_be);
void    print_ssrc(uint32_t ssrc_be);
void    print_recv(uint32_t recv_be);
void    print_len(uint16_t len_be);
int     is_video(rtp_hdr *r);

// ssrc_check.c

bool ssrc_checker(void *content, size_t size, size_t position);

// video_stream.c 

void inspect_video_stream(void *content, size_t size, size_t position);

#endif
