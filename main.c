#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAX_OPACITY (int) (sizeof(levels) / sizeof((levels)[0]) - 1)
#define DEF_OPACITY 13

static const int levels[] = { 0, 10, 20, 30, 35, 40, 45, 50, 55, 60, 
                             65, 70, 75, 80, 85, 90, 93, 96, 100 };
static int get_current(char *);
static int save_current(char *, int);
static void send_escape_sequences(int);
static void help(void);

int main(int argc, char *argv[]) {
    const char *name = "opacity";
    const char *cache = "/tmp";
    int window_id;
    char opacity_file[200];
    int current;

    if (argc <= 1)
        help();
    else
        window_id = (int) getppid(); // in case we don't get argv[2]

    (void) snprintf(opacity_file, sizeof(opacity_file), "%s/%s_%d", cache, name, window_id);

    current = get_current(opacity_file);

    if ((argv[1][0] == '-') && (0 < current))
        current -= 1;
    else if ((argv[1][0] == '+') && (current < MAX_OPACITY))
        current += 1;
    else if (argv[1][0] == '=')
        current = MAX_OPACITY;
    else if (argv[1][0] == 'h')
        help();

    current = save_current(opacity_file, current);
    send_escape_sequences(levels[current]);

    return 0;
}

int get_current(char *cache_name) {
    FILE *cache;
    char current_str[4];
    char *end_pointer;
    int current;

    if (!(cache = fopen(cache_name, "r"))) {
        if(errno != ENOENT) {
            fprintf(stderr, "Can't open file for getting current opacity, keeping urxvt 100%% opaque\n");
            return MAX_OPACITY;
        } else { // cache file doesn't exist yet, so return default value.
            return DEF_OPACITY;
        }
    }
    if (!fgets(current_str, 3, cache)) {
        fprintf(stderr, "Can't read from file, keeping urxvt 100%% opaque\n");
        (void) fclose(cache);
        return MAX_OPACITY;
    }

    current = (int) strtol(current_str, &end_pointer, 10);
    if ((current < 0) || (current > MAX_OPACITY) || (end_pointer == current_str)) {
        fprintf(stderr, "Invalid opacity read from file, keeping urxvt 100%% opaque\n");
        (void) fclose(cache);
        return MAX_OPACITY;
    }

    (void) fclose(cache);
    return current;
}

int save_current(char *cache_name, int wanted) {
    FILE *save;

    if (!(save = fopen(cache_name, "w"))) {
        fprintf(stderr, "Can't open file for saving current opacity, keeping urxvt 100%% opaque\n");
        return MAX_OPACITY;
    }
    if (fprintf(save, "%i\n", wanted) < 0) {
        fprintf(stderr, "Can't write to file, keeping urxvt 100%% opaque\n");
        (void) fclose(save);
        return MAX_OPACITY;
    }

    (void) fclose(save);
    return wanted;
}

void send_escape_sequences(int level) {
    printf("\033]011;[%i]#000000\007", level); //background
    printf("\033]708;[%i]#000000\007", level); //border
}

void help(void) {
    (void) puts("urxvt_opacity [-+=h]\n"
                "- -- decrease\n"
                "+ -- increase\n"
                "= -- set 100% opaque\n"
                "h -- print this help message");
    exit(EXIT_SUCCESS);
}
