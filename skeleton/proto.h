#ifndef PROTO_H
#define PROTO_H

#include <stddef.h>

#define CMD_UPLOAD 1
#define CMD_LIST 2
#define CMD_STATUS 3
#define CMD_QUIT 4

#define MAX_FILENAME 256
#define MAX_MESSAGE 256

typedef struct
{
  int cmd;
  char filename[MAX_FILENAME];
} request_t;

typedef struct
{
  char message[MAX_MESSAGE];
} response_t;

int read_full(int fd, void *buf, size_t count);
int write_full(int fd, const void *buf, size_t count);

int send_request(int fd, const request_t *req);
int recv_request(int fd, request_t *req);

int send_response(int fd, const response_t *res);
int recv_response(int fd, response_t *res);

#endif