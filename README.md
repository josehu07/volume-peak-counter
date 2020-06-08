# MP3 Volume Peak Counter

A tiny tool to display the volume (loudness) curve of an `.mp3` music file and count the number of volume peaks in it. Written in three different languages just for fun (C99, Rust, Go).

Available on Linux or OS X platforms.


## Installation

First, clone this git repo:

```bash
$ git clone ...
```

Then, prepare the prerequisites:

1. `loudness-scanner` is a git submodule by `jiixyj` ([repo](https://github.com/jiixyj/loudness-scanner)) and should be pulled automatically. Follow its installation guide to build a `loudness` binary at `loudness-scaner/build/loudness`. Please make sure one of its input plugins is working.
2. `gnuplot_i` ...

Finally, build the counter by:

```bash
$ make counter-[c|rs|go]   # Choose whatever language version you prefer.
```


## Usage

Run from command line:

```bash
$ ./counter MP3_FILENAME
```


## Licensing

This tool is licensed under the GNU GPL license. See COPYING file for details.
