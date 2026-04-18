#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"

#define DEFAULT_SOCKET_PATH "/tmp/video.sock"

int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_un addr;
    request_t req;
    response_t res;
    const char *socket_path = DEFAULT_SOCKET_PATH;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <video_file>\n", argv[0]);
        return 1;
    }

    if (getenv("VIDEO_SOCKET_PATH") != NULL)
    {
        socket_path = getenv("VIDEO_SOCKET_PATH");
    }

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(fd);
        return 1;
    }

    memset(&req, 0, sizeof(req));
    req.cmd = CMD_UPLOAD;
    strncpy(req.filename, argv[1], sizeof(req.filename) - 1);

    if (send_request(fd, &req) <= 0)
    {
        perror("send_request");
        close(fd);
        return 1;
    }

    if (recv_response(fd, &res) <= 0)
    {
        perror("recv_response");
        close(fd);
        return 1;
    }

    printf("Server: %s\n", res.message);

    close(fd);
    return 0;
}