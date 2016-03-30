/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include <samplerate.h>

#include <jack/jack.h>
#include <jack/thread.h>

#include "c-common/file.h"
#include "c-common/memory.h"
#include "c-common/sound-file.h"
#include "c-common/byte-order.h"
#include "c-common/observe-signal.h"
#include "c-common/jack-ringbuffer.h"
#include "c-common/jack-client.h"
#include "c-common/jack-port.h"

#include "sockets.h"
#include "daemonize.h"

#define NUM_JACK_PORTS 2

struct player_options {
    int buffer_frames;
    int minimal_frames;
    i64 seek_request;
    double seek_request_seq;
    bool transport_aware;
    int unique_name;
    double src_ratio;
    int rb_request_frames;
    int converter;
    char client_name[64];
    char *dst_pattern;
    float ampl;
    bool loop;
    bool paused;
    /* this was originally player */
    int buffer_bytes;
    int buffer_samples;
    float *d_buffer;
    float *j_buffer;
    float *k_buffer;
    SNDFILE *sound_file;
    int channels;
    jack_port_t **output_port;
    float **out;
    jack_ringbuffer_t *rb;
    pthread_t disk_thread;
    pthread_t socket_thread;
    int pipe[2];
    jack_client_t *client;
    SRC_STATE *src;
};


void *disc_proc(void *ptr) {
    /* read sound file from disk and write to ring buffer */
}

void *socket_proc(void *ptr) {
    /* communicate with rtrd */

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

int sync_handler(jack_transport_state_t state, jack_position_t *position, void *PTR) {

}

void main() {
    /* 
     * start jack client
     * read from socket for new songs to play
     */
    daemonize("rtr-jack");

    struct player_options player;
    player.buffer_frames = 4096;
    player.minimal_frames = 32;
    player.seek_request = -1;
    player.seek_request_seq = -1;
    player.transport_aware = false;
    player.unique_name= true;
    player.src_ratio = 1.0;
    player.rb_request_frames = 64;
    player.converter = SRC_SINC_FASTEST;
    player.ampl = 1.0;
    player.dst_pattern = NULL;
    player.loop = false;
    player.paused = false;
    strncpy(player.client_name, "read-the-room", 64);

    /* start thread that opens socket with rtrd */
    /* start jack client */
    observe_signals();

    /* allocate memory for output ports */
    player.out = xmalloc(player.channels * sizeof(float *));
    player.output_port = xmalloc(NUM_JACK_PORTS * sizeof(jack_port_t *));
   
    /* allocate buffers */
    /* in jack-play, the number of channels in a file was used instead
     * of the number of jack ports, but we're going to have 2 jack ports
     * regardless of the number of channels in a file
     */
    player.buffer_samples = player.buffer_samples * NUM_JACK_PORTS;
    player.buffer_bytes = player.buffer_samples * sizeof(float);
    player.d_buffer = xmalloc(player.buffer_bytes);
    player.j_buffer = xmalloc(player.buffer_bytes);
    player.k_buffer = xmalloc(player.buffer_bytes);
    player.rb = jack_ringbuffer_create(player.buffer_bytes);
    
    /* set up samplerate conversion */
    int err;
    player.src = src_callback_new(
        read_from_ringbuffer,
        player.converter,
        NUM_JACK_PORTS,
        &err,
        &player);
    if (!player.src) {
        syslog(LOG_ERR, "Sample rate conversion setup failed: %s", src_strerror(err));
        exit(EXIT_FAILURE);
    }

    /* Create communication pipe */
    xpipe(player.pipe);

    /* Become a client of the jack server */
    if (player.unique_name) {
        player.client = jack_client_unique_store(player.client_name);
    } else {
        player.client = jack_client_open(player.client_name, JackNullOption, NULL);
    }
    if (!player.client) {
        syslog(LOG_ERR, "Could not create jack client: %s", player.client_name);
        exit(EXIT_FAILURE);
    }

    /* start disk thread, priority is a random guess */
    jack_client_create_thread(
        player.client,
        &(player.disk_thread),
        50,
        true,
        disc_proc,
        &player);

    /* TODO: start socket thread */

    /* set error, process and shutdown handlers */
    jack_set_error_function(jack_client_minimal_error_handler);
    jack_on_shutdown(player.client, jack_client_minimal_shutdown_handler, 0);
    if(player.transport_aware) {
        jack_set_sync_callback(player.client, sync_handler, &player);
    }
    jack_set_process_callback(player.client, signal_proc, &player);

    /* create output ports, connect them and activate client */
    jack_port_make_standard(player.client, player.output_port, NUM_JACK_PORTS, true);
    jack_client_activate(player.client);
    if (player.dst_pattern == NULL) {
        /* TODO: use user-set regex here from cli, or the default */
        player.dst_pattern = getenv("JACK_PLAY_CONNECT_TO");
    }
    if (player.dst_pattern) {
        char src_pattern[128];
        snprintf(src_pattern, 128, "%s:out%%d", player.client_name);
        jack_port_connect_pattern(
            player.client,
            NUM_JACK_PORTS,
            0,
            src_pattern,
            player.dst_pattern);
    }

    /* TODO: open sound file */
    /* TODO: inform user of samplerate mismatch */
    /* TODO: translate initial seek request if given in seconds and cancel */

    /* shutdown */
    /* TODO: wait for socket thread to end */

    
    /* wait tor disk thread to end, which it
     * does when it reaches the end of the file or is interrupted. */
    pthread_join(player.disk_thread, NULL);


    jack_client_close(player.client);
    /* TODO: close sound file */

    jack_ringbuffer_free(player.rb);
    close(player.pipe[0]);
    close(player.pipe[1]);
    free(player.d_buffer);
    free(player.j_buffer);
    free(player.k_buffer);
    free(player.out);
    free(player.output_port);
    src_delete(player.src);

    exit(EXIT_SUCCESS);
}
