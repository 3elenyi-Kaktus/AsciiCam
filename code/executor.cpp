#include "executor.h"
#include "termios.h"
#include "scroll_object.h"
#include "chat.h"

#include "thread"
#include "string"

void EnterVideoChat(int option, Terminal &terminal, WebCamera &camera, GUI &interface, Logger &logger) {
    Client client{};
    if (client.Connect(logger) < 0) {
        return;
    }
    if (option == 1) {  // host
        char pass_code[10];
        client.GetMessage(logger); // get passcode from server
        sscanf(client.recv_buf, "%s", pass_code);
        logger << "Got passcode from server: " << pass_code << "\n";

        client.GetMessage(logger); // get confirmation from server that peer connected
        logger << "Connection with peer established\n";
    } else {            // client
        while (true) {
            auto coords = PrintInputMenu(terminal, interface.passcode_enter);
            std::string input = GetInputFromInputMenu(coords);
            logger << "Entered passcode: " << input << "\n";
            snprintf(client.send_buf, sizeof(client.send_buf), "%s", input.c_str());
            client.SendMessage(logger); // send entered passcode to server for confirmation

            client.GetMessage(logger); // get server response
            int accepted;
            sscanf(client.recv_buf, "%d", &accepted);
            if (accepted) {
                logger << "Passcode accepted\n"
                          "Connection with peer established\n";
                break;
            }
            logger << "Passcode denied\n";
        }
    }


    ClearScreen();


    int height = terminal.height;
    int width = terminal.width - 4;
    int companion_height, companion_width;
    int companion_new_height, companion_new_width;
    int other_side_has_camera;

    logger << "Terminal height x width: " << height << "x" << width << "\n";
    logger << "Camera is " << ((camera.is_initialized) ? "" : "not ") << "initialized\n";
    snprintf(client.send_buf, sizeof(client.send_buf), "%d %d %d", height, width, camera.is_initialized);
    client.SendMessage(logger); // send terminal size and camera availability info to peer

    client.GetMessage(logger); // get terminal size and camera availability info from peer
    sscanf(client.recv_buf, "%d %d %d", &companion_height, &companion_width, &other_side_has_camera);
    logger << "Got following companion's height x width: " << companion_height << "x" << companion_width
           << "\n";
    logger << "Companion's camera is " << ((other_side_has_camera) ? "" : "not ") << "initialized\n";
    if (camera.is_initialized) { // calculate to which size frames should be transformed before sending to peer
        camera.GetNewFrame(logger);
        std::pair<int, int> ret_size = camera.GetFittedFrameSize(companion_height, companion_width);
        ret_size = ConvertSizeToSymb(ret_size);
        companion_new_height = ret_size.first;
        companion_new_width = ret_size.second;
        logger << "Companion's frame resulting size is height x width: " << companion_new_height << "x"
               << companion_new_width << "\n";
    } else { // default parameters (for rework in future)
        companion_new_height = 10;
        companion_new_width = 10;
    }
    snprintf(client.send_buf, sizeof(client.send_buf), "%d %d", companion_new_height, companion_new_width);
    client.SendMessage(logger); // send resulting size to peer

    client.GetMessage(logger); // get resulting size from peer
    sscanf(client.recv_buf, "%d %d", &height, &width);
    logger << "Got following frame resulting height x width: " << height << "x" << width << "\n";

    // Enter get/send state, in which in 2 threads:
    // 1. Get frame, Convert frame, Send frame
    // 2. Get other persons frame, Print it
    // Threads are activated only if their existence is appropriate (? not anymore)

    std::mutex mutex;

    std::thread receiver = std::thread([&]() {
        logger << "Created receiving thread\n";
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
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    matrix[i][j] = client.recv_buf[i * width + j];
                }
            }
            mutex.lock();
            PrintFrame(terminal, matrix);
            mutex.unlock();
        }
    });

    std::thread sender = std::thread([&]() {
        logger << "Created sending thread\n";
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
                std::string msg = "here could be frame";
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
        }
    });

    if (sender.joinable()) {
        sender.join();
    }
    if (receiver.joinable()) {
        receiver.join();
    }

    client.TerminateConnection(logger);
}

void EnterChat(int option, Terminal &terminal, GUI &interface, Logger &logger) {
    Client client{};
    if (client.Connect(logger) < 0) {
        return;
    }
    if (option == 1) {  // host
        char pass_code[10];
        client.GetMessage(logger); // get passcode from server
        sscanf(client.recv_buf, "%s", pass_code);
        logger << "Got passcode from server: " << pass_code << "\n";

        client.GetMessage(logger); // get confirmation from server that peer connected
        logger << "Connection with peer established\n";
    } else {            // client
        while (true) {
            auto coords = PrintInputMenu(terminal, interface.passcode_enter);
            std::string input = GetInputFromInputMenu(coords);
            logger << "Entered passcode: " << input << "\n";
            snprintf(client.send_buf, sizeof(client.send_buf), "%s", input.c_str());
            client.SendMessage(logger); // send entered passcode to server for confirmation

            client.GetMessage(logger); // get server response
            int accepted;
            sscanf(client.recv_buf, "%d", &accepted);
            if (accepted) {
                logger << "Passcode accepted\n"
                          "Connection with peer established\n";
                break;
            }
            logger << "Passcode denied\n";
        }
    }

    ClearScreen();

    curs_set(1);
    int pos_y = terminal.height - 2;
    int pos_x = 1;
    int msg_pos_y = 1;
    int msg_pos_x = 1;
    std::string my_msg_pref = "You > ";
    std::string serv_msg_pref = "Server > ";
    std::string ending_message = "The other person terminated chat.";
    std::atomic_bool chat_is_closed = false;
    std::mutex mtx;

    struct termios orig_termios{};  // Save canonical terminal mode and switch to raw
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    std::thread receiver([&]() {
        std::string message;
        while (!chat_is_closed) {
            client.GetMessage(logger);
            message = std::string(client.recv_buf);
            logger << "Got msg from server: " << message << "\n";
            if (message == "quit") {
                chat_is_closed = true;
                message = ending_message;
            }

            mtx.lock();
            mvprintw(msg_pos_y, msg_pos_x, "%s%s", serv_msg_pref.c_str(), message.c_str());
            getyx(stdscr, msg_pos_y, msg_pos_x);
            ++msg_pos_y;
            msg_pos_x = 1;
            move(pos_y, pos_x);
            refresh();
            mtx.unlock();
        }
    });

    std::thread sender([&]() {
        while (!chat_is_closed) {
            std::string input;
            char c;
            while (true) {
                move(pos_y, pos_x);
                refresh();
                if (read(STDIN_FILENO, &c, 1) != 1) {
                    break;
                }
                if (iscntrl(c) && c != 127) { // (c == 10 || c == 13) for Enter keycode check
                    logger << "Control letter entered\n";
                    break;
                }
                logger << "Letter entered: " << c << " " << (int) c << "\n";
                if (c == 127) { // Delete char, if possible
                    if (input.empty()) {
                        continue;
                    }
                    mtx.lock();
                    --pos_x;
                    mvaddch(pos_y, pos_x, ' ');
                    mtx.unlock();
                    input.pop_back();
                } else { // Add char, if possible
                    mtx.lock();
                    mvaddch(pos_y, pos_x, c);
                    ++pos_x;
                    mtx.unlock();
                    input += c;
                }
            }
            logger << "Message entered: " << input << "\n";
            if (input == "quit") {
                chat_is_closed = true;
            }

            mtx.lock();
            pos_y = terminal.height - 2;
            pos_x = 1;
            move(pos_y, pos_x);
            for (int i = 0; i < input.length(); ++i) {
                addch(' ');
            }

            mvprintw(msg_pos_y, msg_pos_x, "%s%s", my_msg_pref.c_str(), input.c_str());
            getyx(stdscr, msg_pos_y, msg_pos_x);
            ++msg_pos_y;
            msg_pos_x = 1;
            move(pos_y, pos_x);
            refresh();
            mtx.unlock();

            snprintf(client.send_buf, sizeof(client.send_buf), "%s", input.c_str());
            logger << "Sending message: " << client.send_buf << "\n";
            client.SendMessage(logger);
        }
    });
    sender.join();
    receiver.join();
    client.TerminateConnection(logger);
    curs_set(0);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // Return original terminal settings
}

void SelfVideo(Terminal &terminal, WebCamera &camera, Logger &logger) {
    camera.GetNewFrame(logger);
    std::pair<int, int> ret_size = camera.GetFittedFrameSize(terminal.height, terminal.width);
    while (true) {
        timeout(1); // wait for keypress
        if (getch() != ERR) {
            break;
        }
        camera.GetNewFrame(logger);
        camera.PreprocessFrame(ret_size);
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
        PrintFrame(terminal, ascii_matrix);
    }
}

void DebSelfVideo(Terminal &terminal, WebCamera &camera, Logger &logger) {
    camera.GetNewFrame(logger);
    std::pair<int, int> ret_size = camera.GetFittedFrameSize(terminal.height, terminal.width);
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

        camera.PreprocessFrame(ret_size);
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


void Execute() {
    std::ofstream file(
            "./cout_cerr.txt"); // <---- somehow, if extracted into function, causes SIGSEGV 139 on call to std::cout

    std::streambuf *old_cout_buf = std::cout.rdbuf(); // save cout/cerr old streambufs
    std::streambuf *old_cerr_buf = std::cerr.rdbuf();

    std::cout.rdbuf(file.rdbuf()); // Redirect cout/cerr to file
    std::cerr.rdbuf(file.rdbuf()); // <----


    Logger logger;
    logger << "Start of AsciiCam\n";
    Terminal terminal;
    GUI interface;
    WebCamera camera(logger);
    while (true) {
        auto coords = PrintMenu(terminal, interface.menu);
        // mvprintw(0, 0, "The number of rows - %d, columns - %d\n", terminal.height, terminal.width);
        int option = GetOption(coords, interface.menu.num_of_options);
        if (option == 1) {
            coords = PrintMenu(terminal, interface.CS_choice);
            option = GetOption(coords, interface.CS_choice.num_of_options);

            if (option == 3) {
                continue;
            } else {
                EnterVideoChat(option, terminal, camera, interface, logger);
            }
        } else if (option == 2) { // unused
            coords = PrintMenu(terminal, interface.CS_choice);
            option = GetOption(coords, interface.CS_choice.num_of_options);

            if (option == 3) {
                continue;
            } else {
                EnterChat(option, terminal, interface, logger);
            }
        } else if (option == 3) { // unused, for parameters
            SelfVideo(terminal, camera, logger);
        } else if (option == 4) { // exit from application
            logger << "Exiting from the application\n";
            break;
        } else if (option == 5) { // for debug
            ClearScreen();

            Chat chat(terminal.height - 2, 0, terminal.width, terminal.height);

            bool is_chatting = true;
            while (is_chatting) {
                int c;
                bool is_typing = true;
                while (is_typing) {
                    c = chat.getChar(logger);
                    is_typing = chat.processInput(c, logger);
                }
                int control_code = chat.processMessage(logger);
                if (control_code == -1) {
                    is_chatting = false;
                }
                if (chat.sendMessage(logger) < 0) {
                    break;
                }
                chat.updateChat();
            }
        }
    }

    std::cout.rdbuf(old_cout_buf); // Restore cout/cerr streambufs before exit
    std::cerr.rdbuf(old_cerr_buf);
}
