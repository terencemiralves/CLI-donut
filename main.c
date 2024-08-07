#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include "msg.h"
#include "render.h"

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
    //Setup the PID stuff
    int pipedfd[2];
    pid_t pid;

    //Create the msg structure that will be used to pass info between the fork and the main program
    struct msg msg;
    msg.size = 10;
    msg.time = 0.1;
    msg.stop = 0;

    if (pipe(pipedfd) == -1)
    {
        perror("pipe");
        return 1;
    }

    //Set the read end of the pipe to non-blocking mode this way I can print without waiting for input
    fcntl(pipedfd[0], F_SETFL, O_NONBLOCK);

    //Set the input reading to not need to press the enter button
    set_nonblocking_input();

    //fork
    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        //In the child read the inputs
        close(pipedfd[0]);
        //Main while loop to read the input and process it
        while (1)
        {
            char c = getchar();
            //To stop
            if (c == 'c')
            {
                msg.stop = 1;
                write(pipedfd[1], &msg, sizeof(msg));
                close(pipedfd[1]);
                exit(0);
            }
            //To change the size of the donut
            else if (c == 'l' || c == 'j')
            {
                msg.size += c == 'l' ? 1 : -1;
                if (msg.size < 0)
                    msg.size = 0;
                write(pipedfd[1], &msg, sizeof(msg));
            }
            //To change the rotation speed
            else if (c == 'a' || c == 'd')
            {
                msg.time += c == 'a' ? 0.015 : -0.015;
                if (msg.time <= 0.0)
                    msg.time = 0.005;
                if (msg.time > 1.0)
                    msg.time = 1.0;
                write(pipedfd[1], &msg, sizeof(msg));
            }
        }
    }
    else 
    {
        // parent
        while (!msg.stop)
        {
            //read what the fork just sent
            read(pipedfd[0], &msg, sizeof(msg));
            //create a speedometer
            char speed[11] = "          ";
            for (int i = 0; i < (int) (10 - (msg.time + 0.005) * 10); i++)
                speed[i] = 'X';
            //check for max value
            if (msg.time <= 0.009)
                speed[9] = 'X';
            //print the donut image
            render(msg.size);
            //print the dashboard
            printf("Speed: [%s]\nSize: %i\nPress j or l to change the size, a or d to change the speed and c to close\n", speed, msg.size);
            if (msg.stop)
                break;
            //sleep
            msleep(msg.time * 1000);
        } 
    }
    //Reset the terminal in order to not block it because of the new settings that we setted for the input reading
    reset_terminal();
    return 0;
}
