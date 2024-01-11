
#include "executor.h"
//TODO:
// Rework requirements. For now, gnome-terminal (as tested platform) and xterm (to resize window programmatically) needed
Logger logger;

//todo
// move all windows in terminal (chat, cam video, etc) to a single class parent as a WINDOW class.
// all created classes must have a RESIZE method
// Terminal must have a list of currently active windows
// on SIGWINCH terminal iterably calls RESIZE on all the classes



int main() {
//    Logger logger;
    logger << "Start of AsciiCam\n";

//    }

//    sigset_t mask;
//    sigfillset(&mask);
//    sigdelset(&mask, SIGUSR1);
//    sigdelset(&mask, SIGUSR2);
//    sigdelset(&mask, SIGTERM);
//    sigprocmask(SIG_BLOCK, &blocker, NULL);
//    sigsuspend(&mask);

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