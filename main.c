#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static const char FILE_NAME[] = ".timenator";

/**
 * Printing tool usage instruction.
 */
void print_usage()
{
    printf("Usage: timenator (start|stop)\n");
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
    printf("event=\"%s\" time=%ld.%09ld\n", e, t->tv_sec, t->tv_nsec);
}

/**
 * Calculate difference and print event, times and difference.
 */
void print_diff(const struct timespec *ct, const struct timespec *t, char *e)
{
    struct timespec d;

    if ((ct->tv_nsec - t->tv_nsec) < 0) {
        d.tv_sec = ct->tv_sec - t->tv_sec - 1;
        d.tv_nsec = ct->tv_nsec - t->tv_nsec + 1000000000;
    } else {
        d.tv_sec = ct->tv_sec - t->tv_sec;
        d.tv_nsec = ct->tv_nsec - t->tv_nsec;
    }

    printf("event=\"%s\" start=%ld.%09ld stop=%ld.%09ld duration=%ld.%09ld\n", e, t->tv_sec, t->tv_nsec, ct->tv_sec, ct->tv_nsec, d.tv_sec, d.tv_nsec);
}

int main(int argc, char *argv[])
{
    struct timespec *ct = get_current_time();
    struct timespec *t;

    if (argc < 2) {
        print_usage();
        exit(0);
    }

    char *event = NULL;

    if (argc > 2) {
        event = argv[2];
    }

    if (strcmp(argv[1], "start") == 0) {
        store_time(ct);
        print_time(ct, event);
        exit(0);
    } else if (strcmp(argv[1], "stop") == 0) {
        t = retrive_time();
        clear_storage();

        if (t == NULL) {
            print_diff(get_current_time(), ct, event);
        } else {
            print_diff(ct, t, event);
        }

        exit(0);
    }

    print_usage();
}
