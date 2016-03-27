# Goal
Write a music player that adapts its playlist depending on what time/day it is.

## Requirements
* uses jackd (v2)
* stereo
* supports at least mp3 and flac, preferably also wav
* does automatic down- and upsampling
* uses a command line tagging tool to assemble playlists (tmsu?)
* reads timing rules from config file (yaml? ini? use crontab timing notation?)
* command line control (play, pause, next, temporary rules?)

## Optional features
* volume also defined in config files (use cronjob for this!)
* volume leveling
* eq
* stop/start playback at set times (use cronjob for this!)


## Approaches

### Easy
Do not write a jack client, just create a script that feeds files to an existing jack audio player.

jack-play might have been suitable, but it only supports wav files. mocp is too unstable for this purpose. mpd might suit, but I dislike how hard it is to add files to the library. aqualung seems feature rich but it has a graphical user interface (it supports some command line instruction). Also; no slackbuild. 

So, no client that fits all my needs here. And lets face it, I am way too intrigued by the idea of writing a jack client to take the easy road.

### Medium
Use either jack-client or py-jack to write this thing in python. It will take some figuring out as documentation is sparse and some threading magic is necessary, but I can get there.

Downside is that there is no way to eliminate locking system calls from functions that have to be realtime safe. So there will be x-runs. Python is not a suitable language for this.

### Hard
Write the entire thing in c. This will mean it will function correctly and efficiently (well, insofar as I can write efficient c code). No more x runs than necessary. Also, we can use proper realtime permissions and stuff because no interpreter.

Downside is that I am not fluent in c. This is going to take a lot of research and trial and error.

#### C libraries
* libsamplerate -> high quality samplerate conversion
* libsndfile -> wav decoding
* libflac -> flac decoding
* libvorbis -> ogg decoding
* libmad -> decode mp3
* jack -> output

#### Useful examples
* jack-play -> ringbuffer use, file playback
* dcron -> parsing cron format rules

#### Architecture
* cli.c - parse command line options and pass them to readtheroom
* readtheroom.c - determine what file to load next, pass to writetheroom
* writetheroom.c - consists of 2 threads (by necessity)
** non-realtime: read file from disk, decode and resample, write to ringbuffer
** realtime: read from ringbuffer, write to jack port

IPC via local sockets.

#### Command line interface
##### Functions
* play/pause
* skip to next song
* start and stop daemon(s)
* display currently playing file (poll daemon status)
* define alternative config file
* define jack regex for automatic port connection

##### cli options
* read-the-room COMMAND(play|pause|next|start|stop|status)
* play -> resume playback
* pause -> pause playback
* start -> start daemons
** -c FILE -> config file
** -j REGEX -> autoconnect to jack ports that match regex
* stop -> stop daemons
* status -> display server status and what song is currently playing

##### planned features
* reload config file
* temporary rule definition


#### Config format parsing
We use the cron format for configuration. Since no parser is available for cpp/c, we create one ourselves using the existing cron libraries.

##### Format
minute hour day_of_month month day_of_week tag_rules

* lines starting with # are ignored (comments)
* values are space-separated, only tag_rules can contain whitespace
* tokens can consist of comma separated sub-tokens
* sub-token can be *, numeric, a range, divisor, any combination of those, or in the case of day_of_week and month, a three-letter name (e.g. mon, feb)

##### Pseudocode
Excuse the half-python, half-c syntax, just wanted to get my thoughts written down without implementation complexity.
```
struct cron_rule_t {
    bool minutes[60];
    bool hours[24];
    bool days_of_month[31];
    bool month[12];
    bool days_of_week[7];
    char* rule[];
}

cron_rule_t* parse_config() {
    cron_rule_t* rules[];
    for line in file:
        if line.startswith("#"): next;
        tokens = line.split(" ");
        # check number of tokens before continuing
        cront_rule_t tmp_rule;
        parse_minutes(tokens[0], tmp_rule);
        parse_hours(tokens[1], tmp_rule);
        parse_days_of_month(tokens[2], tmp_rule);
        parse_month(tokens[3], tmp_rule);
        parse_days_of_week(tokens[4], tmp_rule);
        tmp_rule.rule = tokens[5:];
        rules.append(tmp_rule);
    }
    return rules;
}

bool rule_match(rule, time) {
    if rule.minutes[time.minute] and
        rule.hours[time.hour] and
        rule.days_of_month[time.date - 1] and
        rule.month[time.month - 1] and
        rule.days_of_week[time.weekday - 1]:
            return true;
    return false;
}
```

##### Improvement points
* allow multi-line rules with a backslash escaped newline
* trim whitespace
* convert double spaces to single spaces?
* treat whitespace only lines as empty
