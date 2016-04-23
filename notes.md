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


## Approach
Write an mocp client, so that we don't have to concern ourself with the audio playing complexities.

### Command line interface
#### Functions
* play/pause
* skip to next song
* start and stop daemon(s)
* display currently playing file (poll daemon status)
* define alternative config file
* define jack regex for automatic port connection

#### cli options
* read-the-room COMMAND(play|pause|next|start|stop|status)
* play -> resume playback
* pause -> pause playback
* start -> start daemons
* * -c FILE -> config file
* * -j REGEX -> autoconnect to jack ports that match regex
* stop -> stop daemons
* status -> display server status and what song is currently playing

#### planned features
* volume control
* reload config file
* temporary rule definition

### Communication between daemons
commands separated by newlines

#### rtrd can receive:
##### read-the-room(client) to rtrd(server)
* PAUSE
* PLAY
* NEXT
* CURRENT
* STOP
TODO: add volume control

#### rtrd can send:
##### rtrd(server) to read-the-room(client)
* CURRENT filename
* ACK

### Architecture
#### rtrd
Consists of two threads. An mocp client thread that handles events from mocp and instructs the server what files to play, and a socket server that listens for command line instructions (pause, next, that kind of stuff).

Both threads have acces to the current_song variable, which is protected with a mutex. Inter thread communication happens via a pipe (socket server writes, mocp client reads). Commands that require action from the mocp client, like for example skipping to the next song, are handled that way.

### Config format parsing
We use the cron format for configuration. Since no parser is available for cpp/c, we create one ourselves using the existing cron libraries.

#### Format
minute hour day_of_month month day_of_week tag_rules

* lines starting with # are ignored (comments)
* values are space-separated, only tag_rules can contain whitespace
* tokens can consist of comma separated sub-tokens
* sub-token can be *, numeric, a range, divisor, any combination of those, or in the case of day_of_week and month, a three-letter name (e.g. mon, feb)

#### Pseudocode
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

#### Improvement points
* allow multi-line rules with a backslash escaped newline
* trim whitespace
* convert double spaces to single spaces?
* treat whitespace only lines as empty

#### rules
Rules are space separated lists of tmsu tags, prefixed with -|+|? to indicate their optionality and/or exclusion.

* + -> mandatory: means this tag has to be present
* ? -> optional: means a file will be chose with at least one of the optional tags
* - -> exclusion: files with this tag will be excluded
