#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "msg.h"

struct termios oldt;

//msleep(): Sleep for the requested number of milliseconds.
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

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
    struct msg msg;
    msg.size = 5;
    msg.time = 0.1;
    msg.stop = 0;

    if (pipe(pipedfd) == -1)
    {
        perror("pipe");
        return 1;
    }

    //Set the read end of the pipe to non-blocking mode this way I can print without waiting for input
    fcntl(pipedfd[0], F_SETFL, O_NONBLOCK);

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
                msg.stop = 1;
                write(pipedfd[1], &msg, sizeof(msg));
                close(pipedfd[1]);
            }
            else if (c == 'l' || c == 'j')
            {
                msg.size += c == 'l' ? 1 : -1;
                if (msg.size < 0)
                    msg.size = 0;
                write(pipedfd[1], &msg, sizeof(msg));
            }
            else if (c == 'a' || c == 'd')
            {
                msg.time += c == 'd' ? 0.1 : -0.1;
                if (msg.time < 0.0)
                    msg.time = 0.0;
                if (msg.time > 5.0)
                    msg.time = 5.0;
                write(pipedfd[1], &msg, sizeof(msg));
            }
        }
    }
    else 
    {
        // parent
        while (!msg.stop)
        {
            printf("Press j or l to change the size, a or d to change the speed and c to close\n");
            for (int i = 0; i < msg.size; i++)
                printf("-");
            printf("\n");
            printf("time: %f\n", msg.time);
            read(pipedfd[0], &msg, sizeof(msg));
            if (msg.stop)
                break;
            msleep(msg.time * 1000);
        } 
    }

    reset_terminal();
    return 0;
}
