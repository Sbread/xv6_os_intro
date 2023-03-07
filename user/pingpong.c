//
// Created by sbread on 24.09.22.
//

#include "user/user.h"

void send(int* fd, const char* message, int bytes) {
  close(fd[0]);
  int n = write(fd[1], message, bytes);
  close(fd[1]);
  if (n != bytes) {
    printf("pipe write error");
    exit(1);
  }
}

void receive(int* fd, const char* expected_message, int bytes) {
  char message[bytes];
  close(fd[1]);
  int n, total_read = 0;
  do {
    n = read(fd[0], message, bytes);
    if (n < 0) {
      printf("pipe read error");
      exit(1);
    }
    for (int i = 0; i < n; ++i) {
      if (total_read + i >= bytes ||
          expected_message[total_read + i] != message[i]) {
        printf("received wrong message");
        exit(1);
      }
    }
    total_read += n;
  } while (n != 0);
  close(fd[0]);
  printf("%d: got %s\n", getpid(), expected_message);
}

int main() {
  int fd1[2];
  int fd2[2];
  const char* message_from_parent = "ping";
  const char* message_from_child = "pong";
  if (pipe(fd1) < 0) {
    printf("first pipe() failed\n");
    exit(1);
  }
  if (pipe(fd2) < 0) {
    printf("second pipe() failed\n");
    exit(1);
  }
  int pid = fork();
  if (pid > 0) {
    send(fd1, message_from_parent, strlen(message_from_parent));
    receive(fd2, message_from_child, strlen(message_from_child));
    exit(0);
  } else if (pid == 0) {
    receive(fd1, message_from_parent, strlen(message_from_parent));
    send(fd2, message_from_child, strlen(message_from_child));
    exit(0);
  } else {
    printf("fork error\n");
    exit(-1);
  }
}