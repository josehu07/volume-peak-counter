#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define SCANNER "./loudness-scanner/build/loudness"
#define INTERVAL (0.5)  // Sampling interval in secs.


/**
 * Execute loudess-scaner and redirect output through pipe to parent process.
 */
void
exec_scanner(const char *mp3_file, const int pipefd[]) {
    close(pipefd[0]);   // Close reading end.

    dup2(pipefd[1], 1);     // Redirect stdout to pipe.
    dup2(pipefd[1], 2);     // Redirect stderr to pipe.

    // Execute shell cmd: "$(scanner) dump -m $(interval) $(mp3_file)".
    char interval[8];
    snprintf(interval, 8, "%f", INTERVAL);
    execl(SCANNER, SCANNER, "dump", "-m", interval, mp3_file, NULL);
}


/**
 * Read loudness-scanner results from pipe and parse the results.
 */
void
curve_parser(const int pipefd[]) {
    close(pipefd[1]);   // Close writing end.

    char c;
    char buf[16];
    int idx = 0;
    char *end_ptr;  // For strtof.

    float timestamp = 0.0;  // Current timestamp (sec) in music.

    bool in_peak = false;
    int peak_count = 0;

    // Read scanner results and get loudness float values.
    while (read(pipefd[0], &c, sizeof(c)) != 0) {
        if (c == '\n' || c == '\r') {   // On newline.
            // Every valid float number line gives loudness of the next interval.
            if (idx > 0 && idx < 16) {
                buf[idx] = '\0';    // Parse float value.
                float volume = strtof(buf, &end_ptr);

                if (end_ptr != NULL && end_ptr > buf) {
                    if (volume > -7.0 && !in_peak) {
                        peak_count++;
                        printf("% 3d % 7.1fs %-15.1f\n", peak_count, timestamp, volume);
                        in_peak = true;
                    } else if (in_peak) {
                        in_peak = false;
                    }

                    timestamp += INTERVAL;
                }
            }
            idx = 0;    // Reset buffer index.
        } else if (idx < 15) {  // Else buffer this char.
            buf[idx] = c;
            idx++;
        }
    }
}


/** Main entrance. */
int
main(void) {
    pid_t pid;
    int pipefd[2];

    pipe(pipefd);

    pid = fork();
    if (pid == 0)   // Child process.
        exec_scanner("/Users/jose/Desktop/test-music.mp3", pipefd);
    else            // Parent process.
        curve_parser(pipefd);

    return 0;
}
