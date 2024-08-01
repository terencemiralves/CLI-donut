#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>



struct termios oldt;

//The main issue with input reading in C is that it always wait for the user to press enter, boring and non usefull for us. So I found out a way to remove the need for the enter press and here it is
void set_nonblocking_input() 
{
    struct termios newt;

    // Get current terminal attributes
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);

    // Set new attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

// We need to reset the terminal settings in order to not have to reset the terminal every time the user is done with it
void reset_terminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main() 
{
    int pipedfd[2];
    pid_t pid;
    int size = 5;

    if (pipe(pipedfd) == -1)
    {
        perror("pipe");
        return 1;
    }

    set_nonblocking_input();

    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        //in the child read the inputs
        close(pipedfd[0]);
        while (1)
        {
            char c = getchar();
            if (c == 'c')
            {
                size = -1;
                write(pipedfd[1], &size, sizeof(size));
                close(pipedfd[1]);
            }
            else if (c == 'l' || c == 'j')
            {
                size += c == 'l' ? 1 : -1;
                write(pipedfd[1], &size, sizeof(size));
            }
        }
    }
    else 
    {
        // parent
        while (1)
        {
            printf("Press j or l to change the size and c to close\n");
            for (int i = 0; i < size; i++)
                printf("-");
            printf("\n");
            read(pipedfd[0], &size, sizeof(size));
            if (size == -1)
                break;
        } 
    }

    reset_terminal();
    return 0;
}
