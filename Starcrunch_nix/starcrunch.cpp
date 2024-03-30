/******************************************************************************
Starcrunch
Created by Michael McCanney on June 2021.
Copyright © 2021 Michael McCanney. All rights reserved.

To compile:
    g++ -o starcrunch starcrunch.cpp -I.
    or
    make (if it has one)

To execute
    ./starcrunch


Information:
Based on the original starcrunch I made in 1984 for the Commodore 64
Creates a n line by 40 character area filled with stars that you have
to avoid


******************************************************************************/

#include <iostream>
#include <chrono>
#include <thread>

#include <time.h>
#include <cstring>
#include <unistd.h>
#include <termios.h>

using namespace std;

struct termios orig_termios;
void set_conio_terminal_mode();
void reset_terminal_mode();
int kbhit();
int getch();
int NumDigits(int x);

const char* Color_Back = "\033[94m";         // blue
const char* Color_Fore = "\033[96m";         // cyan

const char* ClearScreen = "\033[2J";
const char* ClearToEol = "\033[0K";         // clear from cursor to end-of-line
const char* ClearToEos = "\033[0J";         // clear from cursor to end-of-screen

const int NumLines = 20;
const int StartingLine = 2;
const int MaxWidth = 40;

void DisplayHelp();

//======================================================================
int main(int argc, const char * argv[]) {

    // clear the screen and move to pos 1,1
    cout << "\e[?1000h";
    cout << ClearScreen << "\033[1;1H";
    //cout << Color_Board << "STARCRUNCH - (c)2021 ZenCat, Michael J. McCanney";// << endl << endl;

    // seed the random number generator
    int seed = time(NULL);
    if (argc == 2) {
        seed = atoi(argv[1]);
        if (seed == 0) seed = time(NULL);
    }
    srand(seed);

    int star_pos[NumLines];   // hold onto each line of star positions
    for (int i = 0; i < NumLines; i++) star_pos[i] = 0;
    char c = 0;
    int player_pos = 20;    // x-coord, there is no y-axis movement of the users character
    int delay_ms = 100;
    int score = 0;
    bool is_esc = false;

    set_conio_terminal_mode();

    cout << "\e[?25l";      // hide the cursor
    cout << Color_Back;
    cout << Color_Fore;
    do {
        // user input
        c = 0; is_esc = false;
        while (kbhit()) {
            c = getch();
            if (c == 27) is_esc = true;
        }
        if (is_esc && (int)c == 68 && player_pos > 1) { player_pos--; }
        else if (is_esc && (int)c == 67 && player_pos < MaxWidth) { player_pos++; }
        else if (c == 'f' && delay_ms > 60) { delay_ms -= 10; }
        else if (c == 's' && delay_ms < 900) { delay_ms += 10; }



        // print the field
        cout << "\033[1;1H" << ClearToEos;
        cout << "\033[1;1H" << "STARCRUNCH (c)2021 ZenCat";
        cout << "\033[1;" << (MaxWidth - NumDigits(score)) << "H" << score;

        for (int i = 0; i < NumLines - 1; i++) {
            if (star_pos[i] > 0) {
                cout << "\033[" << (i + StartingLine) << ";" << star_pos[i] << "H*";    // goto stars position and print
            }
            star_pos[i] = star_pos[i + 1];  // shift the star positions upward as we go
        }
        // the last row, find a new star position and print
        star_pos[NumLines - 1] = (rand() % MaxWidth) + 1;
        cout << "\033[" << (StartingLine + (NumLines-1)) << ";" << star_pos[NumLines - 1] << "H*";

        // goto player position and print, V or # if hit
        if (player_pos == star_pos[0]) {
            cout << "\033[" << StartingLine << ";" << player_pos << "H#";
            c = 'x';
        }
        else {
            cout << "\033[" << StartingLine << ";" << player_pos << "HV";
            score += 10;
        }
        cout.flush();

        // slow down by adding a small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

    } while (c != 'q' && c != 'x');
    cout << "\e[?25h";      // unhide the cursor

    if (c == 'x') {
        cout << "\033[3;1HCRUNCHED!";
    }
    cout << "\033[" << (NumLines + StartingLine) << ";1H";   // goto the end of the star field
    reset_terminal_mode();

    return 0;
}

void DisplayHelp() {
    cout << "STARCRUNCH" << endl;
    cout << "   (c)2021 Michael J. McCanney" << endl;
    cout << endl;
    cout << " Avoid the stars or you'll go CRUNCH!" << endl;
    cout << endl;
    cout << " Left arrow  = Move LEFT" << endl;
    cout << " Right arrow = Move RIGHT" << endl;
    cout << " f           = Faster" << endl;
    cout << " s           = Slower" << endl;
    cout << " q           = Quit" << endl;
    cout << " ?           = Show this help screen" << endl;
}

int NumDigits(int x) {
    string s = std::to_string(x);
    return s.length();
}
void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    // take two copies - one for now, one for later 
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    // register cleanup handler, and set the new terminal mode
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}
