#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Signal handler function
void handle_signal(int signal) {
    switch (signal) {
        case SIGHUP:
            printf("Received SIGHUP (Hangup detected on controlling terminal or death of controlling process). Ignoring...\n");
            break;
        case SIGINT:
            printf("Received SIGINT (Interrupt from keyboard). Exiting...\n");
            exit(0);
        case SIGQUIT:
            printf("Received SIGQUIT (Quit from keyboard). Exiting...\n");
            exit(0);
        case SIGILL:
            printf("Received SIGILL (Illegal Instruction). Exiting...\n");
            exit(0);
        case SIGABRT:
            printf("Received SIGABRT (Abort signal from abort(3)). Exiting...\n");
            exit(0);
        case SIGFPE:
            printf("Received SIGFPE (Floating-point exception). Exiting...\n");
            exit(0);
        case SIGKILL:
            printf("Received SIGKILL (Kill signal). Exiting...\n");
            exit(0);
        case SIGSEGV:
            printf("Received SIGSEGV (Invalid memory reference). Exiting...\n");
            exit(0);
        case SIGPIPE:
            printf("Received SIGPIPE (Broken pipe: write to pipe with no readers). Ignoring...\n");
            break;
        case SIGALRM:
            printf("Received SIGALRM (Timer signal from alarm(2)). Ignoring...\n");
            break;
        case SIGTERM:
            printf("Received SIGTERM (Termination signal). Exiting...\n");
            exit(0);
        case SIGUSR1:
            printf("Received SIGUSR1 (User-defined signal 1). Ignoring...\n");
            break;
        case SIGUSR2:
            printf("Received SIGUSR2 (User-defined signal 2). Ignoring...\n");
            break;
        case SIGCHLD:
            printf("Received SIGCHLD (Child stopped or terminated). Ignoring...\n");
            break;
        case SIGCONT:
            printf("Received SIGCONT (Continue if stopped). Ignoring...\n");
            break;
        case SIGSTOP:
            printf("Received SIGSTOP (Stop process). Ignoring...\n");
            break;
        case SIGTSTP:
            printf("Received SIGTSTP (Stop typed at terminal). Ignoring...\n");
            break;
        case SIGTTIN:
            printf("Received SIGTTIN (Terminal input for background process). Ignoring...\n");
            break;
        case SIGTTOU:
            printf("Received SIGTTOU (Terminal output for background process). Ignoring...\n");
            break;
        default:
            printf("Received unknown signal %d. Ignoring...\n", signal);
            break;
    }
}

int main() {
    // Register signal handler
    signal(SIGHUP, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGILL, handle_signal);
    signal(SIGABRT, handle_signal);
    signal(SIGFPE, handle_signal);
    signal(SIGKILL, handle_signal);
    signal(SIGSEGV, handle_signal);
    signal(SIGPIPE, handle_signal);
    signal(SIGALRM, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);
    signal(SIGCHLD, handle_signal);
    signal(SIGCONT, handle_signal);
    signal(SIGSTOP, handle_signal);
    signal(SIGTSTP, handle_signal);
    signal(SIGTTIN, handle_signal);
    signal(SIGTTOU, handle_signal);

    // Keep the program running
    while (1) {
        printf("Running... Press Ctrl+C to exit.\n");
        sleep(1);
    }

    return 0;
}