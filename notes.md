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
