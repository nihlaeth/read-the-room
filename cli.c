/*
 * Copyright 2016 Nihlaeth
 */
#include <stdlib.h>
#include <argp.h>

const char *argp_program_version = "read-the-room 0.0";
const char *argp_program_bug_address = "<read-the-room@nihlaeth.nl>";

/* Program documentation. */
static char doc[] =
    "read-the-room -- a music player that adjusts the playlist with time\n"
    "    COMMAND - play|pause|start|stop\n"
    "        start - start daemon (optinally specify -j and -c options)\n"
    "        stop - stop daemon\n"
    "        pause - pause playback\n"
    "        play - resume playback\n"
    "        status - display daemon status and currently playing song";

/* A description of the arguments we accept. */
static char args_doc[] = "COMMAND";

/* The options we understand. */
static struct argp_option options[] = {
    {"jack_ports", 'j', "REGEX", 0, "regex for port autoconnection (default: \"^system:playback_[0-9]$\")" },
    {"config_file", 'c', "FILE", 0, "configuration file (default: ~/readtheroomrc)" },
    { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char *args[1];
    char *jack_ports;
    char *config_file;
};

/* Parse a single option */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
         know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'c':
            arguments->config_file = arg;
            break;
        case 'j':
            arguments->jack_ports = arg;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
                /* Too many arguments. */
                argp_usage(state);
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                /* Not enough arguments. */
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
        }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

void main(int argc, char **argv) {
    struct arguments arguments;

    /* Default values. */
    arguments.config_file = "~/readtheroomrc";
    arguments.jack_ports = "^system:playback_[0-9]$";

    /* Parse our arguments; every option seen by parse_opt will
         be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    printf("COMMAND = %s\nCONFIG_FILE = %s\n JACK_PORTS = %s\n",
        arguments.args[0], arguments.args[1],
        arguments.config_file,
        arguments.jack_ports);

    exit(EXIT_SUCCESS);

}
