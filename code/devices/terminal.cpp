#include "terminal.h"
#include <csignal>
#include <sys/ioctl.h>
#include "thread"

Terminal::Terminal() : screen() {
    screen.keycodesMode(true);
    screen.cursorMode(0);
    screen.echoMode(0);

//    struct sigaction sig_handler = {.sa_flags = SA_RESTART};
//    sig_handler.sa_handler = SIGWINCH_handler;
//    sigaction(SIGWINCH, &sig_handler, nullptr);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGWINCH);
    pthread_sigmask(SIG_BLOCK, &sigset, nullptr);


    logger << "Blocked SIGWINCH\n";

    std::thread resizer = std::thread([&]() {
        while (true) {
            int signum = 0;
            sigwait(&sigset, &signum);
            std::cerr << "\n\nCAUGHT SIGNAL\n";

            screen.block();
            struct timespec timer(0, 150000000);
            while (sigtimedwait(&sigset, nullptr, &timer) >= 0) {
                std::cerr << "\n\nSLEEPING\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(150));

            }
            screen.unblock();

            struct winsize sizew{};
            if (ioctl(fileno(stdout), TIOCGWINSZ, &sizew) == 0) {
                std::cerr << "TERM NEW SIZE " << sizew.ws_row << "x" << sizew.ws_col << "\n";
                resize_term(sizew.ws_row, sizew.ws_col);
            }

            screen.Resize();
            Size new_size = screen.getSize();
            for (const std::shared_ptr<Window> &window: active_windows) {
                window->Resize(new_size);
            }
            screen.refreshScreen(true);
        }
    });
    resizer.detach();
}

void SIGWINCH_handler(int a) {
    resize_signal_catched = true;
}