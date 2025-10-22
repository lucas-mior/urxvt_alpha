#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdint.h>

char *program;
#include "util.c"

#define SNPRINTF(BUFFER, FORMAT, ...)                                          \
    snprintf2(BUFFER, sizeof(BUFFER), FORMAT, __VA_ARGS__)
#define MAX_OPACITY (int)(sizeof(levels) / sizeof(*levels) - 1)
#define DEF_OPACITY 13

static void help(FILE *) __attribute__((noreturn));

int
main(int argc, char *argv[]) {
    static const int levels[] = {0,  10, 20, 30, 35, 40, 45, 50, 55, 60,
                                 65, 70, 75, 80, 85, 90, 93, 96, 100};
    const char *name = "opacity";
    const char *cache = "/tmp";
    int window_id;
    char opacity_file[256];
    int current;

    if (argc <= 1) {
        help(stderr);
    }
    program = argv[0];

    window_id = (int)getppid();

    SNPRINTF(opacity_file, "%s/%s_%d", cache, name, window_id);
    do {
        int cache2;
        ssize_t r;
        char current_str[16];

        if ((cache2 = open(opacity_file, O_RDONLY)) < 0) {
            if (errno != ENOENT) {
                error("Can't open file for getting current opacity. "
                      "Keeping urxvt 100%% opaque\n");
                current = MAX_OPACITY;
                break;
            } else {
                current = DEF_OPACITY;
                break;
            }
        }
        if ((r = read(cache2, current_str, sizeof(current_str))) <= 0) {
            error("Can't read from %s", opacity_file);
            if (r < 0) {
                error(": %s", strerror(errno));
            }
            error(".\nKeeping urxvt 100%% opaque.\n");

            if (close(cache2) < 0) {
                error("Error closing %s: %s.\n", opacity_file, strerror(errno));
            }
            current = MAX_OPACITY;
            break;
        }

        current = atoi(current_str);
        if ((current < 0) || (current > MAX_OPACITY)) {
            error("Invalid opacity read from file. "
                  "Keeping urxvt 100%% opaque\n");
            current = MAX_OPACITY;
        }

        close(cache2);
    } while (0);

    if ((argv[1][0] == '-') && (0 < current)) {
        current -= 1;
    } else if ((argv[1][0] == '+') && (current < MAX_OPACITY)) {
        current += 1;
    } else if (argv[1][0] == '=') {
        current = MAX_OPACITY;
    } else if (argv[1][0] == 'h') {
        help(stdout);
    }

    do {
        FILE *save;

        if (!(save = fopen(opacity_file, "w"))) {
            error("Can't open file for saving current opacity. "
                  "Keeping urxvt 100%% opaque\n");
            current = MAX_OPACITY;
            break;
        }
        if (fprintf(save, "%i\n", current) <= 0) {
            error("Can't write to file, keeping urxvt 100%% opaque\n");
            fclose(save);
            current = MAX_OPACITY;
        }

        fclose(save);
    } while (0);

    printf("\033]011;[%i]#000000\007", levels[current]);  // background
    printf("\033]708;[%i]#000000\007", levels[current]);  // border

    exit(EXIT_SUCCESS);
}

void
help(FILE *stream) {
    fprintf(stream, "urxvt_alpha [-+=h]\n"
                    "- -- decrease\n"
                    "+ -- increase\n"
                    "= -- set 100%% opaque\n"
                    "h -- print this help message");
    exit(stream != stdout);
}
