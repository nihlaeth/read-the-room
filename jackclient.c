/*
 * Copyright 2016 Nihlaeth
 */
#include <jack/jack.h>
#include <jack/thread.h>

struct player_options {

};

struct player {

};

void *disc_proc(void *ptr) {
    /* read sound file from disk and write to ring buffer */
}

void play_file(const char *file_name) {
    /*
     * setup for song - allocate buffers, get channel and rate conversion info
     */

}

long read_from_ringbuffer(void *ptr, float **buf){
    /*
     * get data from ringbuffer, return n of frames read.
     */
}

int signal_proc(jack_nframes_t nframes, void *ptr) {
    /* write data from ringbutfer to jack output ports. */

}

void main() {
    /* 
     * start jack client
     * read from socket for new songs to play
     */


}
