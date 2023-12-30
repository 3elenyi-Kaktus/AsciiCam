#include "executor.h"

//TODO:
// Rework requirements. For now, gnome-terminal (as tested platform) and xterm (to resize window programmatically) needed
Logger logger;

int main() {
//    Logger logger;
    logger << "Start of AsciiCam\n";

    setlocale(LC_ALL, "");
    try {
        Execute();
    } catch (const std::exception &e) {
        logger << e.what();
    } catch (...) {
        logger << "Unknown exception occurred\n";
    }

    logger << "Quitting AsciiCam\n";
    return 0;
}