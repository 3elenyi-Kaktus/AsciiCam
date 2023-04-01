#include "executor.h"


void Execute() {
    Logger logger;
    logger << "Start of AsciiCam\n";
    Terminal terminal = InitTerminalWindow();
    GUI interface;
    WebCamera camera(logger);
    while (true) {
        //mvprintw(terminal.ROWS - 1, 0, "The number of rows - %d, columns - %d\n", terminal.ROWS, terminal.COLS);
        int option = PrintMenu(terminal, interface.menu);
        if (option == 1) {
            // Enter get/send state, in which in 2 threads:
            // 1. Get frame, Convert frame, Send frame
            // 2. Get other persons frame, Print it
            option = PrintMenu(terminal, interface.CS_choice);
            ClearScreen();
            if (option == 1) { // Create new server connection
                Host host(logger);
                logger << "Connected to client\n";
                host.GetMessage(logger);
                int width, height, new_height, new_width;
                char *end;
                char *end2;
                height = strtol(host.buffer, &end, 10);
                width = strtol(end, &end2, 10);
                logger << "Got following height x width from client: " << height << "x" << width << "\n";
                camera.GetNewFrame(logger);
                camera.PreprocessFrame(height, width);
                new_height = camera.GetFrame().rows;
                new_width = camera.GetFrame().cols;
                logger << "Frame preprocessing returned following height x width: " << new_height << "x" << new_width
                       << "\n";
                snprintf(host.buffer, sizeof(host.buffer), "%d %d", new_height, new_width);
                host.SendMessage(logger);
                logger << "Sent input frame size to client\n";
                while (true) {
                    timeout(1); // wait for keypress
                    if (getch() != ERR) {
                        break;
                    }
                    logger << "getting new frame\n";
                    camera.GetNewFrame(logger);
                    camera.PreprocessFrame(height, width);
                    std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
                    for (int i = 0; i < ascii_matrix.size(); ++i) {
                        for (int j = 0; j < ascii_matrix[0].size(); ++j) {
                            host.buffer[i * ascii_matrix[0].size() + j] = ascii_matrix[i][j];
                        }
                    }
                    host.SendMessage(logger);
                }
                host.TerminateConnection();
            } else {        // Create new client connection
                Client client(logger);
                logger << "Connected to host\n";
                int height = terminal.height;
                int width = terminal.width - 4;
                snprintf(client.buffer, sizeof(client.buffer), "%d %d", height, width);
                client.SendMessage(logger);
                logger << "Sent height x width to host: " << height << "x" << width << "\n";
                client.GetMessage(logger);
                char *end;
                char *end2;
                height = strtol(client.buffer, &end, 10);
                width = strtol(end, &end2, 10);
                logger << "Got following input frame height x width from client: " << height << "x" << width << "\n";
                while (true) {
                    timeout(1); // wait for keypress
                    if (getch() != ERR) {
                        break;
                    }
                    client.GetMessage(logger);
                    logger << "got new msg\n";
                    std::vector<std::vector<u_char>> matrix(height, std::vector<u_char>(width));
                    for (int i = 0; i < height; ++i) {
                        for (int j = 0; j < width; ++j) {
                            matrix[i][j] = client.buffer[i * width + j];
                        }
                    }
                    PrintFrame(terminal, matrix);
                }

                client.TerminateConnection();
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
