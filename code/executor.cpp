#include "executor.h"
#include "webcam_capture.h"
#include "ascii_converter.h"

void Execute() {
    Terminal terminal = InitTerminalWindow();
    GUI interface;
    while (true) {
        //mvprintw(terminal.ROWS - 1, 0, "The number of rows - %d and columns - %d\n", terminal.ROWS, terminal.COLS);
        int option = PrintMenu(terminal, interface.menu);
        if (option == 1) {
            // Enter get/send state, in which in 2 threads:
            // 1. Get frame, Convert frame, Send frame
            // 2. Get other persons frame, Print it


            CamVideo(terminal); // Some dummy self-printing camera script
        } else if (option == 2) {   // unused
        } else if (option == 3) {   // unused, for parameters
        } else if (option == 4) {
            TerminateTerminalWindow();
            break;
        }
    }
}

void CamVideo(Terminal& terminal) {
    while (true) {
        timeout(1); // wait for keypress
        if (getch() != ERR) {
            break;
        }
        cv::Mat frame = GetFrame();
        cv::resize(frame, frame, cv::Size(), 2, 2, cv::INTER_AREA);
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(frame);
        PrintFrame(terminal, ascii_matrix);
    }
}
