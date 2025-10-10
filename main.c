#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>

#define SNPRINTF(BUFFER, FORMAT, ...) \
    snprintf2(BUFFER, sizeof(BUFFER), FORMAT, __VA_ARGS__)
#define MAX_OPACITY (int) (sizeof(levels) / sizeof(*levels) - 1)
#define DEF_OPACITY 13

typedef int32_t int32;

static const int levels[] = { 0, 10, 20, 30, 35, 40, 45, 50, 55, 60, 
                             65, 70, 75, 80, 85, 90, 93, 96, 100 };
static int save_current(char *, int);
static void help(FILE *) __attribute__((noreturn));
static void *snprintf2(char *, size_t, char *, ...);
static void error(char *, ...);

int main(int argc, char *argv[]) {
    const char *name = "opacity";
    const char *cache = "/tmp";
    int window_id;
    char opacity_file[256];
    int current;

    if (argc <= 1)
        help(stderr);
    else
        window_id = (int) getppid(); // in case we don't get argv[2]

    SNPRINTF(opacity_file, "%s/%s_%d", cache, name, window_id);
    do {
        FILE *cache2;
        char current_str[4];
        char *endptr;

        if (!(cache2 = fopen(opacity_file, "r"))) {
            if (errno != ENOENT) {
                error("Can't open file for getting current opacity. "
                      "Keeping urxvt 100%% opaque\n");
                current = MAX_OPACITY;
            } else {
                current = DEF_OPACITY;
            }
        }
        if (!fgets(current_str, 3, cache2)) {
            error("Can't read from file, keeping urxvt 100%% opaque\n");
            (void) fclose(cache2);
            current = MAX_OPACITY;
        }

        current = (int) strtol(current_str, &endptr, 10);
        if ((current < 0) || (current > MAX_OPACITY) || (endptr == current_str)) {
            error("Invalid opacity read from file. "
                            "Keeping urxvt 100%% opaque\n");
            (void) fclose(cache2);
            current = MAX_OPACITY;
        }

        (void) fclose(cache2);
    } while (0);

    if ((argv[1][0] == '-') && (0 < current))
        current -= 1;
    else if ((argv[1][0] == '+') && (current < MAX_OPACITY))
        current += 1;
    else if (argv[1][0] == '=')
        current = MAX_OPACITY;
    else if (argv[1][0] == 'h')
        help(stdout);

    current = save_current(opacity_file, current);
    printf("\033]011;[%i]#000000\007", levels[current]); //background
    printf("\033]708;[%i]#000000\007", levels[current]); //border

    return 0;
}

int save_current(char *cache_name, int wanted) {
    FILE *save;

    if (!(save = fopen(cache_name, "w"))) {
        error("Can't open file for saving current opacity. "
                        "Keeping urxvt 100%% opaque\n");
        return MAX_OPACITY;
    }
    if (fprintf(save, "%i\n", wanted) < 0) {
        error("Can't write to file, keeping urxvt 100%% opaque\n");
        (void) fclose(save);
        return MAX_OPACITY;
    }

    (void) fclose(save);
    return wanted;
}

void help(FILE *stream) {
    fprintf(stream, "urxvt_alpha [-+=h]\n"
                    "- -- decrease\n"
                    "+ -- increase\n"
                    "= -- set 100%% opaque\n"
                    "h -- print this help message");
    exit(stream != stdout);
}

void *
snprintf2(char *buffer, size_t size, char *format, ...) {
    int n;
    va_list args;

    va_start(args, format);
    n = vsnprintf(buffer, size, format, args);
    va_end(args);

    if (size <= 8) {
        error("%s: wrong buffer size = %zu.\n", __func__, size);
        exit(EXIT_FAILURE);
    }
    if (n >= (int)size) {
        va_list args2;
        buffer = malloc((size_t)n + 1);
        va_start(args, format);
        va_copy(args2, args);
        n = vsnprintf(buffer, (size_t)n + 1, format, args);
        va_end(args);
    }
    if (n <= 0) {
        error("Error in snprintf.\n");
        exit(EXIT_FAILURE);
    }
    return buffer;
}

void
error(char *format, ...) {
    int32 n;
    va_list args;
    char buffer[BUFSIZ];

    va_start(args, format);
    n = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (n < 0) {
        fprintf(stderr, "Error in vsnprintf()\n");
        exit(EXIT_FAILURE);
    }
    if (n > (int32)sizeof(buffer)) {
        fprintf(stderr, "Error in vsnprintf: buffer is not large enough.\n");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    write(STDERR_FILENO, buffer, (size_t)n);
    fsync(STDERR_FILENO);
    fsync(STDOUT_FILENO);
    return;
}
