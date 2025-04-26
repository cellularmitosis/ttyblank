/* ttyblank: a tty blanker. */
/* written by Gemini AI. */
/* This code is released into the public domain. */

/* usage: either run this manually, or replace 'agetty' in your /etc/inittab. */

/* build instructions: gcc -o ttyblank ttyblank.c */

#include <stdio.h>      // Standard I/O functions (printf, perror)
#include <stdlib.h>     // Standard library functions (exit, atexit)
#include <unistd.h>     // POSIX operating system API (read, isatty, STDOUT_FILENO, STDIN_FILENO)
#include <termios.h>    // POSIX terminal control definitions

// ANSI Escape Codes
#define CLEAR_SCREEN "\033[2J" // Clears the entire screen
#define CURSOR_HOME  "\033[H"  // Moves cursor to top-left (row 1, col 1)
#define HIDE_CURSOR  "\033[?25l" // Hides the cursor
#define SHOW_CURSOR  "\033[?25h" // Shows the cursor

// Global variable to store original terminal settings
struct termios original_termios;
int original_stdin_flags; // To store original stdin flags if needed (not strictly needed here)

// Function to restore terminal settings and cursor on exit
void restore_terminal_and_cursor(void) {
    // Show the cursor first, before restoring terminal modes
    if (write(STDOUT_FILENO, SHOW_CURSOR, sizeof(SHOW_CURSOR) - 1) == -1) {
        perror("Warning: Could not write show cursor code");
        // Continue anyway, try to restore termios
    }

    // Restore the original terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) < 0) {
        perror("Error restoring terminal settings (tcsetattr)");
    }
}

// Function to disable raw mode and echo
void enable_raw_mode() {
    struct termios raw;

    // Get the current terminal attributes
    if (tcgetattr(STDIN_FILENO, &original_termios) < 0) {
        perror("Error getting terminal settings (tcgetattr)");
        exit(EXIT_FAILURE);
    }

    // Ensure restoration happens on exit (normal or via exit())
    // Register the combined restoration function
    atexit(restore_terminal_and_cursor);

    // Copy original settings to modify
    raw = original_termios;

    // Disable canonical mode (ICANON), echo (ECHO), signal chars (ISIG), Ctrl-V (IEXTEN)
    raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);

    // Disable software flow control (IXON, IXOFF), CR->NL map (ICRNL), etc.
    raw.c_iflag &= ~(IXON | IXOFF | ICRNL | INPCK | ISTRIP | BRKINT);
    raw.c_cflag |= (CS8); // Ensure 8-bit characters
    raw.c_oflag &= ~(OPOST); // Disable output processing

    // Set read() to return after 1 char, no timeout
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    // Apply the modified attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        perror("Error setting terminal to raw mode (tcsetattr)");
        // atexit() should still try to restore if possible
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Check if standard input is connected to a terminal
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Error: Not running in a terminal.\n");
        return EXIT_FAILURE;
    }

    // Set terminal to raw mode (disables echo, line buffering)
    enable_raw_mode();

    // Clear screen, move cursor home, and hide cursor
    // Use write() for robustness, especially if OPOST is disabled
    if (write(STDOUT_FILENO, CLEAR_SCREEN, sizeof(CLEAR_SCREEN) - 1) == -1) {
         perror("Error writing clear screen code");
         return EXIT_FAILURE;
    }
     if (write(STDOUT_FILENO, CURSOR_HOME, sizeof(CURSOR_HOME) - 1) == -1) {
         perror("Error writing cursor home code");
         return EXIT_FAILURE;
     }
     if (write(STDOUT_FILENO, HIDE_CURSOR, sizeof(HIDE_CURSOR) - 1) == -1) {
         perror("Error writing hide cursor code");
         return EXIT_FAILURE;
     }

    // Wait for a single keypress
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        perror("Error reading input key");
        // The atexit handler will still attempt to restore settings & cursor
        return EXIT_FAILURE;
    }

    // The atexit handler 'restore_terminal_and_cursor' is called automatically.

    return EXIT_SUCCESS; // Normal exit
}
