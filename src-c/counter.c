#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../gnuplot_i/src/gnuplot_i.h"
#include "ring-buf.h"


#define SCANNER "./loudness-scanner/build/loudness"
#define INTERVAL (0.5)  // Sampling interval in secs.

#define WINDOW_SIZE (128)
#define SLEEP_LENGTH (0.1)  // Sleep only 0.1 sec for every 0.5 secs of music.

#define YRANGE_LOW (-20)
#define YRANGE_HIGH (-2)
#define THRESHOLD (-7.0)


/**
 * Produce a gnuplot plot.
 */
void
plot_volume_curve(gnuplot_ctrl *h, char *mp3_file,
                  RingBuf *volume_window, RingBuf *timestamp_window,
                  int peak_count, float latest_peak_timestamp) {
    double xs[WINDOW_SIZE];
    double ys[WINDOW_SIZE];

    int count = 0;
    for (int view = ring_buf_view(volume_window);
         view != -1;
         view = ring_buf_next(volume_window, view)) {
        xs[count] = ring_buf_read(timestamp_window, view);
        ys[count] = ring_buf_read(volume_window, view);
        count++;
    }

    gnuplot_resetplot(h);   // Reset to simulate an animation.

    gnuplot_setstyle(h, "linespoints");
    gnuplot_set_xlabel(h, "Timestamp (s)");
    gnuplot_set_ylabel(h, "Volume (LUFS)");

    // Dynamic axis range.
    double xs_low, xs_high;
    if (count < WINDOW_SIZE - 1) {
        xs_low = xs[count - 1] - (INTERVAL * (WINDOW_SIZE - 2));
        xs_high = xs[count - 1];
    } else {
        xs_low = xs[0];
        xs_high = xs[count - 1];
    }
    gnuplot_cmd(h, "set xrange [%lf:%lf]", xs_low, xs_high);
    gnuplot_cmd(h, "set yrange [%d:%d]", YRANGE_LOW, YRANGE_HIGH);

    // Volume line (smoothed).
    gnuplot_plot_xy(h, xs, ys, count, mp3_file,
                    "smooth acsplines lt rgb 'dark-blue' lw 2");

    // Threshold line.
    gnuplot_cmd(h, "set arrow from %f,%f to %f,%f nohead %s",
                xs_low, THRESHOLD, xs_high, THRESHOLD,
                "lt rgb 'dark-red' lw 1");

    // Customized text.
    char latest_peak_text[16];
    if (peak_count > 0) {
        snprintf(latest_peak_text, 16, "%.1fs", latest_peak_timestamp);
        // gnuplot_cmd(h, "set arrow from %f,%f to %f,%f %s",  // Vertical.
        //             latest_peak_timestamp, YRANGE_LOW,
        //             latest_peak_timestamp, THRESHOLD,
        //             "lt rgb 'red' lw 2");
    } else
        snprintf(latest_peak_text, 16, "none");
    gnuplot_cmd(h, "unset label");
    gnuplot_cmd(h, "set label '# Peaks: %-3d; Latest: %s' left %s",
                peak_count, latest_peak_text,
                "at graph 0.05,0.9 font 'Verdana,18'");

    usleep((int) (SLEEP_LENGTH * 1e6));     // Sleep to give proper animation.
}


/**
 * Execute loudess-scaner and redirect output through pipe to parent process.
 */
void
exec_scanner(char *mp3_file, const int pipefd[]) {
    close(pipefd[0]);   // Close reading end.

    dup2(pipefd[1], 1);     // Redirect stdout to pipe.
    dup2(pipefd[1], 2);     // Redirect stderr to pipe.

    // Execute shell cmd: "$(scanner) dump -m $(interval) $(mp3_file)".
    char interval[8];
    snprintf(interval, 8, "%f", INTERVAL);
    execl(SCANNER, SCANNER, "dump", "-m", interval, mp3_file, NULL);
}


/**
 * Read loudness-scanner results from pipe and parse the results. Plot the
 * volume curve through gnuplot_i along the way.
 */
void
curve_parser(gnuplot_ctrl *h, char *mp3_file, const int pipefd[]) {
    close(pipefd[1]);   // Close writing end.

    char c;
    char pipe_buf[16];
    int pipe_idx = 0;
    char *end_ptr;  // For strtof.

    float timestamp = 0.0;  // Current timestamp (sec) in music.

    bool in_peak = false;
    int peak_count = 0;
    float latest_peak_timestamp = -1.0;

    RingBuf *volume_window = ring_buf_new(WINDOW_SIZE);
    RingBuf *timestamp_window = ring_buf_new(WINDOW_SIZE);

    // Read scanner results and get loudness float values.
    while (read(pipefd[0], &c, sizeof(c)) != 0) {
        // On newline.
        if (c == '\n' || c == '\r') {
            // Every valid float number gives loudness of the next interval.
            if (pipe_idx > 0 && pipe_idx < 16) {
                pipe_buf[pipe_idx] = '\0';  // Parse float value.
                float volume = strtof(pipe_buf, &end_ptr);

                if (end_ptr != NULL && end_ptr > pipe_buf) {
                    if (volume > THRESHOLD && !in_peak) {
                        in_peak = true;
                    } else if (in_peak) {
                        peak_count++;
                        latest_peak_timestamp = timestamp;
                        in_peak = false;
                    }

                    // Take snapshot of volume window here.
                    ring_buf_push(volume_window, volume);
                    ring_buf_push(timestamp_window, timestamp);
                    plot_volume_curve(h, mp3_file,
                                      volume_window, timestamp_window,
                                      peak_count, latest_peak_timestamp);

                    timestamp += INTERVAL;
                }
            }
            pipe_idx = 0;    // Reset buffer index.

        // Else buffer this char.
        } else if (pipe_idx < 15) {
            pipe_buf[pipe_idx] = c;
            pipe_idx++;
        }
    }

    ring_buf_del(volume_window);
    ring_buf_del(timestamp_window);
}


/** Main entrance. */
int
main(void) {
    gnuplot_ctrl *h = gnuplot_init();   // Maintain a persistent session.

    char *mp3_file = "test-songs/棱镜-摇晃.mp3";    // Hardcoded for now.

    pid_t pid;
    int pipefd[2];

    pipe(pipefd);

    pid = fork();
    if (pid == 0)   // Child process.
        exec_scanner(mp3_file, pipefd);
    else            // Parent process.
        curve_parser(h, mp3_file, pipefd);

    gnuplot_close(h);
    return 0;
}
