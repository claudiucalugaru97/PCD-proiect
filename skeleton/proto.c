#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "proto.h"

int read_full(int fd, void *buf, size_t count)
{
  size_t done = 0;
  char *p = (char *)buf;

  while (done < count)
  {
    ssize_t rc = read(fd, p + done, count - done);
    if (rc < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }
      return -1;
    }
    if (rc == 0)
    {
      return 0;
    }
    done += (size_t)rc;
  }

  return (int)done;
}

int write_full(int fd, const void *buf, size_t count)
{
  size_t done = 0;
  const char *p = (const char *)buf;

  while (done < count)
  {
    ssize_t rc = write(fd, p + done, count - done);
    if (rc < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }
      return -1;
    }
    if (rc == 0)
    {
      return 0;
    }
    done += (size_t)rc;
  }

  return (int)done;
}

int send_request(int fd, const request_t *req)
{
  return write_full(fd, req, sizeof(*req));
}

int recv_request(int fd, request_t *req)
{
  return read_full(fd, req, sizeof(*req));
}

int send_response(int fd, const response_t *res)
{
  return write_full(fd, res, sizeof(*res));
}

int recv_response(int fd, response_t *res)
{
  return read_full(fd, res, sizeof(*res));
}