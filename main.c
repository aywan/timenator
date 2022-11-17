#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<stdarg.h>

static const char FILE_NAME[] = ".timenator";

struct timespec *ct;

/**
 * Printing tool usage instruction.
 */
void print_usage()
{
    printf("Usage: timenator (start|stop)\n");
}

void print_log(const char* format, ...)
{
    time_t now;
    time(&now);

    char time_str[sizeof "2011-10-08T07:07:09Z"];
    strftime(time_str, sizeof time_str, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    va_list arg;
    va_start(arg, format);

    printf("[%s] ", time_str);
    vprintf(format, arg);
    puts("");

    va_end(arg);
}

/**
 * Storing last time.
 */
int store_time(const struct timespec *t)
{
    FILE *f = fopen(FILE_NAME, "wb+");
    fwrite(t, 1, sizeof(struct timespec), f);
    fclose(f);
    return 0;
}

/**
 * Get current system monotonic time.
 */
struct timespec *get_current_time()
{
    struct timespec *t = malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, t);
    return t;
}

/**
 * Get time from storage by key.
 *
 * @return null or *timespec.
 */
struct timespec *retrive_time()
{
    FILE *f = fopen(FILE_NAME, "rb");
    if (f == NULL) {
        return NULL;
    }
    struct timespec *time = malloc(sizeof(struct timespec));
    fread(time, 1, sizeof(struct timespec), f);
    fclose(f);
    return time;
}

/**
 * Delete storage
 */
void clear_storage()
{
    remove(FILE_NAME);
}

/**
 * Printing time with event
 */
void print_time(const struct timespec *t, char *e)
{
    print_log("event=\"%s\" time=%ld.%09ld\n", e, t->tv_sec, t->tv_nsec);
}

/**
 * Calculate difference and print event, times and difference.
 */
void print_diff(const struct timespec *current, const struct timespec *t, char *e)
{
    struct timespec d;

    if ((current->tv_nsec - t->tv_nsec) < 0) {
        d.tv_sec = current->tv_sec - t->tv_sec - 1;
        d.tv_nsec = current->tv_nsec - t->tv_nsec + 1000000000;
    } else {
        d.tv_sec = current->tv_sec - t->tv_sec;
        d.tv_nsec = current->tv_nsec - t->tv_nsec;
    }

    print_log(
        "event=\"%s\" start=%ld.%09ld stop=%ld.%09ld duration=%ld.%09ld",
        e,
        t->tv_sec,
        t->tv_nsec,
        current->tv_sec,
        current->tv_nsec,
        d.tv_sec,
        d.tv_nsec
    );
}

int cmd_start(char *event) {
    store_time(ct);
    print_time(ct, event);

    return 0;
}

int cmd_stop(char *event) {
    struct timespec *t;

    t = retrive_time();
    if (t == NULL) {
        t = get_current_time();
        print_diff(t, ct, event);
    } else {
        print_diff(ct, t, event);
    }


    clear_storage();

    return 0;
}

int main(int argc, char *argv[])
{
    ct = get_current_time();

    if (argc < 2) {
        print_usage();
        exit(0);
    }

    char *event = NULL;

    if (argc > 2) {
        event = argv[2];
    }

    if (strcmp(argv[1], "start") == 0) {
        return cmd_start(event);
    } else if (strcmp(argv[1], "stop") == 0) {
        return cmd_stop(event);
    }

    print_usage();
}
