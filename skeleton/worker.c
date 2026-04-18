#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static void spawn_ffmpeg_scale(const char *input, const char *output, const char *scale)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char vf_arg[64];
        snprintf(vf_arg, sizeof(vf_arg), "scale=%s", scale);

        char *args[] = {
            "ffmpeg",
            "-i", (char *)input,
            "-vf", vf_arg,
            "-y",
            (char *)output,
            NULL};

        execvp("ffmpeg", args);
        perror("execvp ffmpeg");
        _exit(1);
    }
}

static void spawn_ffmpeg_thumbs(const char *input, int interval)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char fps_arg[64];
        snprintf(fps_arg, sizeof(fps_arg), "fps=1/%d", interval);

        char *args[] = {
            "ffmpeg",
            "-i", (char *)input,
            "-vf", fps_arg,
            "-y",
            "thumb_%03d.jpg",
            NULL};

        execvp("ffmpeg", args);
        perror("execvp ffmpeg thumbnails");
        _exit(1);
    }
}

void process_video(const char *input_file, const char *res1, const char *res2, int thumb_interval)
{
    char out1[256];
    char out2[256];

    snprintf(out1, sizeof(out1), "out_%s.mp4", strcmp(res1, "1280:720") == 0 ? "720p" : "res1");
    snprintf(out2, sizeof(out2), "out_%s.mp4", strcmp(res2, "854:480") == 0 ? "480p" : "res2");

    spawn_ffmpeg_scale(input_file, out1, res1);
    spawn_ffmpeg_scale(input_file, out2, res2);
    spawn_ffmpeg_thumbs(input_file, thumb_interval);

    while (wait(NULL) > 0)
    {
    }
}