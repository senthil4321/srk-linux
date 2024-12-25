#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>

void check_capabilities() {
    cap_t caps;
    char *caps_text;

    // Get the current capabilities
    caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc");
        exit(EXIT_FAILURE);
    }

    // Convert capabilities to text
    caps_text = cap_to_text(caps, NULL);
    if (caps_text == NULL) {
        perror("cap_to_text");
        cap_free(caps);
        exit(EXIT_FAILURE);
    }

    // Print the capabilities
    printf("Current capabilities: %s\n", caps_text);

    // Free allocated memory
    cap_free(caps_text);
    cap_free(caps);
}

int main() {
    printf("Hello, World!\n");

    // Check and print current capabilities
    check_capabilities();

    return 0;
}