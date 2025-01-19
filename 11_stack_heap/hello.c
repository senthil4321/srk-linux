#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(void)
{
        int i = 0;
        char *str = NULL;
        str = strdup("Hello World");
        if (str == NULL)
        {
               fprintf(stderr, "Error \n");
               return (-1);
        }
        for (i = 0; i < 1000; i++)
        {
                printf("[%d] %s (%p)\n", i, str, &str);
                sleep(1);
        }
        return (0);
}