#include "executor.h"
#include "webcam_capture.h"
#include "ascii_converter.h"
#include "logger.h"

Logger logger;

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
            option = PrintMenu(terminal, interface.CS_choice);
            if (option == 1) {
                // Create new server connection
            } else {
                // Create new client connection
            }
        } else if (option == 2) {   // unused
        } else if (option == 3) {   // unused, for parameters
            CamVideo(terminal); // Some dummy self-printing camera script
        } else if (option == 4) {
            TerminateTerminalWindow();
            break;
        }
    }
}

void CamVideo(Terminal &terminal) {
    while (true) {
        auto start1 = std::chrono::high_resolution_clock::now();
        timeout(1); // wait for keypress
        if (getch() != ERR) {
            break;
        }
        auto start = std::chrono::high_resolution_clock::now();
        cv::Mat frame = GetFrame();
        auto end = std::chrono::high_resolution_clock::now();
        logger << "Frame was taken in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
               << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        cv::cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
        end = std::chrono::high_resolution_clock::now();
        logger << "Frame was grayscaled in "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        cv::resize(frame, frame, cv::Size(), 2, 2, cv::INTER_AREA);
        end = std::chrono::high_resolution_clock::now();
        logger << "Frame was resized in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
               << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(frame);
        end = std::chrono::high_resolution_clock::now();
        logger << "Frame was converted in "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        PrintFrame(terminal, ascii_matrix);
        end = std::chrono::high_resolution_clock::now();
        logger << "Frame was printed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
               << "ms\n";
        end = std::chrono::high_resolution_clock::now();
        logger << "All cycle done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start1).count()
               << "ms\n";
    }
}
