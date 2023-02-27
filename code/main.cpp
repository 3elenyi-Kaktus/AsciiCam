#include "printer.h"


int main() {
    initscr();
    Terminal terminal;
    Setup(&terminal);
    PrintMenu(terminal);
    endwin();
    return 0;
}