#include <stdio.h>
#include <unistd.h>
#include <termios.h>



struct termios oldt;

//The main issue with input reading in C is that it always wait for the user to press enter, boring and non usefull for us. So I found out a way to remove the need for the enter press and here it is
void set_nonblocking_input() {
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

int main() {
    char c;
    set_nonblocking_input();
    while (1)
    {
        printf("Press any key to continue...\n");
        c = getchar();
        printf("\nYou pressed: %c\n", c);
        if (c == 'c')
            break;
    }
    reset_terminal();
    return 0;
}
