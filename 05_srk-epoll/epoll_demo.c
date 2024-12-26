#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENTS 10

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void epoll_demo() {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    // Example: monitoring stdin (fd 0)
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLIN) {
                char buf[512];
                ssize_t count = read(events[i].data.fd, buf, sizeof(buf));
                if (count == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                buf[count] = '\0';
                printf("Read: %s", buf);
            }
        }
    }

    close(epoll_fd);
}

int main() {
    set_nonblocking(STDIN_FILENO);
    epoll_demo();
    return 0;
}
