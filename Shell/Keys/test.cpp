#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main (void)
{
    unsigned char buff [6];
    unsigned int x, y, btn;
    struct termios original, raw;

    // Save original serial communication configuration for stdin
    tcgetattr( STDIN_FILENO, &original);

    // Put stdin in raw mode so keys get through directly without
    // requiring pressing enter.
    cfmakeraw (&raw);
    tcsetattr (STDIN_FILENO, TCSANOW, &raw);

    // Switch to the alternate buffer screen
//    write (STDOUT_FILENO, "\e[?47h", 6);

    // Enable mouse tracking
//    write (STDOUT_FILENO, "\e[?9h", 5);
    while (1) {
        read (STDIN_FILENO, &buff, 1);
        printf ("button:'%u'\n", buff[0]);
        if (buff[0] == 3) {
            // User pressd Ctr+C
	    printf("\r\n");
            break;
        } else if (buff[0] == '\x1B') {
            // We assume all escape sequences received 
            // are mouse coordinates
            int n = read (STDIN_FILENO, &buff, sizeof(buff));
	    for (int x=0; x<n; ++x) 
		    printf("%d: %u,", x, buff[x]);
	    printf("\r\n");
        }
	else {
	    printf("\r\n");
	}
    }

    // Revert the terminal back to its original state
//    write (STDOUT_FILENO, "\e[?9l", 5);
//    write (STDOUT_FILENO, "\e[?47l", 6);
    tcsetattr (STDIN_FILENO, TCSANOW, &original);
    return 0;
}

