#include <stddef.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <libconfig.h>

#include "proto.h"

extern void process_video(const char *input_file, const char *res1, const char *res2, int thumb_interval);

#define DEFAULT_SOCKET_PATH "/tmp/video.sock"
#define DEFAULT_RES1 "1280:720"
#define DEFAULT_RES2 "854:480"
#define DEFAULT_THUMB_INTERVAL 10

static int unix_stream_socket(const char *filename)
{
  struct sockaddr_un name;
  int sock;

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("socket");
    return -1;
  }

  memset(&name, 0, sizeof(name));
  name.sun_family = AF_UNIX;
  strncpy(name.sun_path, filename, sizeof(name.sun_path) - 1);

  unlink(filename);

  if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
  {
    perror("bind");
    close(sock);
    return -1;
  }

  if (listen(sock, 10) < 0)
  {
    perror("listen");
    close(sock);
    return -1;
  }

  return sock;
}

static void load_config(char *socket_path, size_t socket_path_len,
                        char *res1, size_t res1_len,
                        char *res2, size_t res2_len,
                        int *thumb_interval)
{
  config_t cfg;
  const char *tmp;

  strncpy(socket_path, DEFAULT_SOCKET_PATH, socket_path_len - 1);
  socket_path[socket_path_len - 1] = '\0';

  strncpy(res1, DEFAULT_RES1, res1_len - 1);
  res1[res1_len - 1] = '\0';

  strncpy(res2, DEFAULT_RES2, res2_len - 1);
  res2[res2_len - 1] = '\0';

  *thumb_interval = DEFAULT_THUMB_INTERVAL;

  config_init(&cfg);

  if (!config_read_file(&cfg, "config.cfg"))
  {
    fprintf(stderr, "Warning: could not read config.cfg, using defaults\n");
    config_destroy(&cfg);
    return;
  }

  if (config_lookup_string(&cfg, "server.socket_path", &tmp))
  {
    strncpy(socket_path, tmp, socket_path_len - 1);
    socket_path[socket_path_len - 1] = '\0';
  }

  if (config_lookup_string(&cfg, "video.res1", &tmp))
  {
    strncpy(res1, tmp, res1_len - 1);
    res1[res1_len - 1] = '\0';
  }

  if (config_lookup_string(&cfg, "video.res2", &tmp))
  {
    strncpy(res2, tmp, res2_len - 1);
    res2[res2_len - 1] = '\0';
  }

  config_lookup_int(&cfg, "video.thumb_interval", thumb_interval);

  config_destroy(&cfg);
}

static void handle_client(int client_fd,
                          const char *res1,
                          const char *res2,
                          int thumb_interval)
{
  request_t req;
  response_t res;

  memset(&req, 0, sizeof(req));
  memset(&res, 0, sizeof(res));

  if (recv_request(client_fd, &req) <= 0)
  {
    perror("recv_request");
    close(client_fd);
    _exit(1);
  }

  switch (req.cmd)
  {
  case CMD_UPLOAD:
  {
    printf("UPLOAD request for file: %s\n", req.filename);

    pid_t pid = fork();
    if (pid == 0)
    {
      process_video(req.filename, res1, res2, thumb_interval);
      _exit(0);
    }

    snprintf(res.message, sizeof(res.message),
             "Processing started for %s", req.filename);
    break;
  }

  case CMD_LIST:
    snprintf(res.message, sizeof(res.message),
             "Available resolutions: 720p, 480p");
    break;

  case CMD_STATUS:
    snprintf(res.message, sizeof(res.message),
             "Server running; workers process uploads asynchronously");
    break;

  default:
    snprintf(res.message, sizeof(res.message),
             "Unknown command");
    break;
  }

  if (send_response(client_fd, &res) <= 0)
  {
    perror("send_response");
  }

  close(client_fd);
  _exit(0);
}

void *unix_main(void *args)
{
  char socket_path[108];
  char res1[64];
  char res2[64];
  int thumb_interval;
  int server_fd;

  (void)args;

  load_config(socket_path, sizeof(socket_path),
              res1, sizeof(res1),
              res2, sizeof(res2),
              &thumb_interval);

  server_fd = unix_stream_socket(socket_path);
  if (server_fd < 0)
  {
    pthread_exit(NULL);
  }

  printf("UNIX server started on %s\n", socket_path);
  printf("Configured resolutions: %s and %s\n", res1, res2);
  printf("Thumbnail interval: %d seconds\n", thumb_interval);

  while (1)
  {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
      perror("accept");
      continue;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
      close(server_fd);
      handle_client(client_fd, res1, res2, thumb_interval);
    }

    close(client_fd);

    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
  }

  close(server_fd);
  unlink(socket_path);
  pthread_exit(NULL);
}