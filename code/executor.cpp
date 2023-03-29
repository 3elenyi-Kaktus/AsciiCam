#include "executor.h"


void Execute() {
    Logger logger;
    Terminal terminal = InitTerminalWindow();
    GUI interface;
    WebCamera camera(logger);
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
            CamVideo(terminal, camera, logger); // Some dummy self-printing camera script
        } else if (option == 4) {
            TerminateTerminalWindow();
            break;
        }
    }
}

void CamVideo(Terminal &terminal, WebCamera &camera, Logger &logger) {
    while (true) {
        auto start1 = std::chrono::high_resolution_clock::now();
        timeout(1); // wait for keypress
        if (getch() != ERR) {
            break;
        }
        auto start = std::chrono::high_resolution_clock::now();
        camera.GetNewFrame(logger);
        auto end = std::chrono::high_resolution_clock::now();
        logger << "Frame was taken in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
               << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        camera.PreprocessFrame(43, 132);
        end = std::chrono::high_resolution_clock::now();
        logger << "Frame was remapped in "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
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
