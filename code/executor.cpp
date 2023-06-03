#include "executor.h"

#include "thread"
#include "string"

void Execute() {
    std::fstream file;
    file.open("cout_cerr.txt", std::ios::out);
    std::string line;

    // Get the streambuffer of the file
    std::streambuf* stream_buffer_file = file.rdbuf();

    // Redirect cout/cerr to file
    std::cout.rdbuf(stream_buffer_file);
    std::cerr.rdbuf(stream_buffer_file);

    Logger logger;
    logger << "Start of AsciiCam\n";
    Terminal terminal;
    GUI interface;
    WebCamera camera(logger);
    while (true) {
        //mvprintw(terminal.ROWS - 1, 0, "The number of rows - %d, columns - %d\n", terminal.ROWS, terminal.COLS);
        int option = PrintMenu(terminal, interface.menu);
        if (option == 1) {
            option = PrintMenu(terminal, interface.CS_choice);
            Client client{};
            if (client.Connect(logger) < 0) {
                break;
            }
            if (option == 1) { // host
                int pass_code;
                client.GetMessage(logger);
                sscanf(client.recv_buf, "%d", &pass_code);
                logger << "Got passcode from server: " << pass_code << "\n";

                client.GetMessage(logger);
                logger << "Connection with peer established\n";
            } else { // client
                while (true) {
                    std::string input = PrintInputMenu(terminal, interface.passcode_enter);
                    logger << "Entered passcode: " << input << "\n";
                    for (int i = 0; i < input.length(); ++i) {
                        client.send_buf[i] = input[i];
                    }
                    client.send_buf[input.length()] = '\0';
                    client.SendMessage(logger);
                    client.GetMessage(logger);
                    int accepted;
                    sscanf(client.recv_buf, "%d", &accepted);
                    if (accepted) {
                        logger << "Passcode accepted\n";
                        break;
                    }
                    logger << "Passcode denied\n";
                }
                logger << "Connection with peer established\n";
            }




            ClearScreen();


            int height = terminal.height;
            int width = terminal.width - 4;
            int companion_height, companion_width, companion_new_height, companion_new_width;
            bool other_side_has_camera;

            logger << "Terminal height x width: " << height << "x" << width << "\n";
            logger << "Camera is " << ((camera.is_initialized) ? "" : "not ") << "initialized\n";
            snprintf(client.send_buf, sizeof(client.send_buf), "%d %d %d", height, width, camera.is_initialized);
            client.SendMessage(logger);
            client.GetMessage(logger);

            sscanf(client.recv_buf, "%d %d %d", &companion_height, &companion_width, &other_side_has_camera);
//            char *end;
//            char *end2;
//            companion_height = strtol(client.recv_buf, &end, 10);
//            companion_width = strtol(end, &end2, 10);
//            other_side_has_camera = strtol(end2, &end, 10);
            logger << "Got following companion's height x width: " << companion_height << "x" << companion_width << "\n";
            logger << "Companion's camera is " << ((other_side_has_camera) ? "" : "not ") << "initialized\n";
            if (camera.is_initialized) {
                camera.GetNewFrame(logger);
                std::pair<int, int> ret_size = camera.GetFittedFrameSize(companion_height, companion_width);
                ret_size = ConvertSizeToSymb(ret_size);
                companion_new_height = ret_size.first;
                companion_new_width = ret_size.second;
                logger << "Companion's frame resulting size is height x width: " << companion_new_height << "x"
                       << companion_new_width << "\n";
            } else {
                companion_new_height = 10;
                companion_new_width = 10;
            }
            snprintf(client.send_buf, sizeof(client.send_buf), "%d %d", companion_new_height, companion_new_width);
            client.SendMessage(logger);
            client.GetMessage(logger);

            sscanf(client.recv_buf, "%d %d", &height, &width);
//            height = strtol(client.recv_buf, &end, 10);
//            width = strtol(end, &end2, 10);
            logger << "Got following frame resulting height x width: " << height << "x" << width << "\n";
            // Enter get/send state, in which in 2 threads:
            // 1. Get frame, Convert frame, Send frame
            // 2. Get other persons frame, Print it
            // Threads are activated only if their existence is appropriate (? not anymore)
            std::mutex mutex;

            std::thread receiver;
//            if (other_side_has_camera) {
                logger << "Created receiving thread\n";
                receiver = std::thread([&]() {
                    int n = 0;
                    while (true) {
                        mutex.lock();
                        timeout(1); // wait for keypress
                        if (getch() != ERR) {
                            mutex.unlock();
                            break;
                        }
                        mutex.unlock();
                        if (client.GetMessage(logger) < 0) {
                            break;
                        }
                        std::vector<std::vector<u_char>> matrix(height, std::vector<u_char>(width));
//                        logger << client.recv_buf << "\n\n";
                        for (int i = 0; i < height; ++i) {
                            for (int j = 0; j < width; ++j) {
                                matrix[i][j] = client.recv_buf[i * width + j];
                            }
                        }
                        mutex.lock();
                        PrintFrame(terminal, matrix);
                        mutex.unlock();
                        ++n;
                    }
                });
//            }
            std::thread sender;
//            if (camera.is_initialized) {
                logger << "Created sending thread\n";
                sender = std::thread([&]() {
                    int n = 0;
                    std::pair<int, int> size = std::make_pair(companion_new_height, companion_new_width);
                    size = ConvertSizeToPx(size);
                    while (true) {
                        mutex.lock();
                        timeout(1); // wait for keypress
                        if (getch() != ERR) {
                            mutex.unlock();
                            break;
                        }
                        mutex.unlock();
                        if (camera.is_initialized) {
                            camera.GetNewFrame(logger);
                            camera.PreprocessFrame(size);
                            std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
                            for (int i = 0; i < ascii_matrix.size(); ++i) {
                                for (int j = 0; j < ascii_matrix[0].size(); ++j) {
                                    client.send_buf[i * ascii_matrix[0].size() + j] = ascii_matrix[i][j];
                                }
                            }
                        } else {
                            std::string msg = "here could be frame n." + std::to_string(n);
                            for (int i = 0; i < msg.length(); ++i) {
                                client.send_buf[i] = msg[i];
                            }
                            for (int i = msg.length(); i < BUF_SIZE; ++i) {
                                client.send_buf[i] = ' ';
                            }
                        }
                        if (client.SendMessage(logger) < 0) {
                            break;
                        }
                        ++n;
                    }
                });
//            }

            if (sender.joinable()) {
                sender.join();
            }
            if (receiver.joinable()) {
                receiver.join();
            }

            client.TerminateConnection(logger);

        } else if (option == 2) {
//            option = PrintMenu(terminal, interface.CS_choice);
            ClearScreen();

            unsigned int available_threads = std::thread::hardware_concurrency();
            logger << available_threads << " concurrent threads are supported.\n";

            echo();
            curs_set(1);
            char a[100];
            int cur_height = 0;
            int cur_width = 0;
            std::string my_message = "You > ";
            std::string ending_message = "The other person terminated chat.";


            Client client{};
            if (client.Connect(logger)) {
                break;
            }

            logger << "Connected to host\n";

            std::string server_message = "Server > ";
            std::atomic_bool chat_is_closed = false;

            std::thread receiver([&]() {
                while (!chat_is_closed) {
                    client.GetMessage(logger);

                    for (int i = 0; i < 80; ++i) {
                        a[i] = client.send_buf[i];
                    }
                    logger << "Got msg from server: " << a << "\n";
                    if (*a == 'q') {
                        chat_is_closed = true;
                        strcpy(a, ending_message.c_str());
                    }

                    int temp_height, temp_width;
                    getyx(stdscr, temp_height, temp_width);
                    mvprintw(cur_height, 0, "%s%s", server_message.c_str(), a);
                    getyx(stdscr, cur_height, cur_width);
                    move(temp_height, temp_width);
                    refresh();
                    ++cur_height;
                }
            });

            std::thread sender([&]() {
                while (!chat_is_closed) {
                    move(terminal.height - 2, 1);
                    getstr(a);
                    logger << "You entered: " << a << "\n";
                    mvprintw(cur_height, 0, "%s%s", my_message.c_str(), a);
                    refresh();
                    getyx(stdscr, cur_height, cur_width);
                    ++cur_height;

                    if (*a == 'q') {
                        chat_is_closed = true;
                    }

                    for (int i = 0; i < 100; ++i) {
                        client.send_buf[i] = a[i];
                    }
                    logger << "sending message: " << client.send_buf << "\n";
                    client.SendMessage(logger);
                }
            });
            sender.join();
            client.TerminateConnection(logger);
            receiver.join();

        } else if (option == 3) {   // unused, for parameters
        } else if (option == 4) {
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
        std::pair<int, int> sz = std::make_pair(43, 132);
        camera.PreprocessFrame(sz);
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
