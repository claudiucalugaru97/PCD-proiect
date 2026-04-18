#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *unix_main(void *args);

int main(void)
{
  pthread_t unixthr;

  if (pthread_create(&unixthr, NULL, unix_main, NULL) != 0)
  {
    perror("pthread_create");
    return 1;
  }

  pthread_join(unixthr, NULL);
  return 0;
}