

#include "main.h"

t_file_content    *read_file(char *filename)
{
    int fd;
    ssize_t size;
    struct stat s;
    void *filecontent;
    t_file_content *f;

    fd = open(filename, O_RDONLY, 0777);
    if (fd == -1)
        return NULL;

    size = fstat(fd, &s);

    filecontent = malloc(s.st_size);
    if (filecontent == NULL)
    {
        close(fd);
        return NULL;
    }
    
    size = read(fd, filecontent, s.st_size);
    if (size == -1)
    {
        close(fd);
        return NULL;
    }

    close(fd);
    f = (t_file_content *)malloc(sizeof(t_file_content));
    if (f == NULL)
    {
        printf("filecontent\n");
        free(filecontent);
        return NULL;
    }

    f->content = filecontent;
    f->size = s.st_size; 
    return f;

}


void    traverse_binary(void *content, size_t size, size_t position)
{
    mjr_frame_hdr *frame; 
    rtp_hdr *r;
    uint32_t first_recv_be = 0;
    uint32_t last_recv_be = 0;
    uint32_t first_ts = 0;
    uint32_t last_ts = 0;

    int i = 0;
    while (position < size)
    {
        // grabe the frame header.
        frame = (mjr_frame_hdr*)content;

        // grab the RTP packet header immediately after it.
        r = (rtp_hdr *)(content + sizeof(mjr_frame_hdr));

        // ensure the first bytes of this frame immediately start with "MEET"
        if (memcmp(frame, "MEET", 4) != 0) 
        {
            printf("incorrect block");
            return ;
        }

        if (i == 1)
        {
            printf("frame->recv_be is %u\n",   frame->recv_be);
            first_recv_be = ntohl(frame->recv_be);  
            first_ts = ntohl(r->ts_be);
        }

        uint16_t len = ntohs(frame->len_be);

        if (position + sizeof(mjr_frame_hdr) + len > size)
            break;                             

        position += sizeof(mjr_frame_hdr) + len;
        content += sizeof(mjr_frame_hdr);
        i++;
        content += len;
    }

    last_ts = ntohl(r->ts_be);
    last_recv_be = ntohl(frame->recv_be);  
    printf("first recv_ be is %u\n", first_recv_be);
    printf("last recv_ be is %u\n", last_recv_be);
    printf("total recv is %u\n", (last_recv_be - first_recv_be)/1000);

    printf("total ts is %f\n", (double)(last_ts - first_ts) / 48000.0);
}

void    unpack_mjr(void *content, size_t size)
{
    char marker[9];
    uint16_t json_len;
    void *intro_ptr;
    void *frame_head_ptr;
    t_drift *drift_list;
    size_t position;
    

    intro_ptr = content;
    bzero(marker, 9);
    memcpy(marker, content, 8);

    // verify that the immediate first 8 bytes are MJR00002
    if (strcmp(marker, MJR_MAGIC) != 0) {
        printf("invalid magic number");
        return ;
    }
    
    // move our pointer buffer over the MJROOOO2 string
    intro_ptr += 8;


    // read the json_leng value 
    json_len = *(uint16_t *)intro_ptr;    
    json_len = ntohs(json_len);         

    printf("json length is %u\n", json_len);

    // first skip the json_len bit byte value itself
    intro_ptr += sizeof(uint16_t);         // skip the 2-byte length

    // then skip "json_len" number of bytes immediately after that.
    intro_ptr += json_len;   
    
    frame_head_ptr = intro_ptr;

    position = 8 + sizeof(uint16_t) + json_len;

    // generic first pass of the binary. Picking potential obvious issues.
    traverse_binary(intro_ptr, size, position);
    
    // zeroing in sequence drift detected in inital first pass.
    drift_list = drift_detection(frame_head_ptr, size, position);

    print_frame_array(frame_head_ptr, size, position, drift_list);

    ssrc_checker(frame_head_ptr, size, position);
    
    inspect_video_stream(frame_head_ptr, size, position);
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: .mjr file\n");
        return -1;
    }
    
    t_file_content *f;

    f = read_file(argv[1]);
    if (f == NULL)
    {
        printf("read file error\n");
        return -1;
    }

    unpack_mjr(f->content, f->size );

    free(f->content);
    free(f);
    return 0;
}