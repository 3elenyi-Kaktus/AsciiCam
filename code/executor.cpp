#include "executor.h"
#include "scroll_object.h"
#include "chat.h"
#include "video_driver.h"

#include "thread"
#include "string"
#include "memory"

void EnterVideoChat(int option, Terminal &terminal, WebCamera &camera, GUI &interface) {
    Network network({"tech", "chat", "video"});
    if (network.Connect() < 0) {
        return;
    }
    if (option == 1) {  // host
        std::unique_ptr<std::string> pass_code;
        network.GetMessage("tech", pass_code); // get passcode from server
        logger << "Got passcode from server: " << *pass_code << "\n";

        network.GetMessage("tech", pass_code); // get confirmation from server that peer connected
        logger << "Connection with peer established\n";
    } else {            // client
        while (true) {
            auto coords = PrintInputMenu(terminal, interface.passcode_enter);
            std::string input = GetInputFromInputMenu(coords);
            logger << "Entered passcode: " << input << "\n";
            network.SendMessage("tech", input); // send entered passcode to server for confirmation

            std::unique_ptr<std::string> reply;
            network.GetMessage("tech", reply); // get server response
            int accepted = std::stoi(*reply);
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
    std::string msg =
            std::to_string(height) + " " + std::to_string(width) + " " + std::to_string(camera.is_initialized);
    network.SendMessage("tech", msg); // send terminal size and camera availability info to peer

    std::unique_ptr<std::string> reply;
    network.GetMessage("tech", reply); // get terminal size and camera availability info from peer
    std::istringstream translator(*reply);
    translator >> companion_height >> companion_width >> other_side_has_camera;
    logger << "Got following companion's height x width: " << companion_height << "x" << companion_width << "\n";
    logger << "Companion's camera is " << ((other_side_has_camera) ? "" : "not ") << "initialized\n";
    if (camera.is_initialized) { // calculate to which size frames should be transformed before sending to peer
        camera.GetNewFrame();
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
    msg = std::to_string(companion_new_height) + " " + std::to_string(companion_new_width);
    network.SendMessage("tech", msg); // send resulting size to peer

    network.GetMessage("tech", reply); // get resulting size from peer
    translator.clear();
    translator.str(*reply);
    translator >> height >> width;
    logger << "Got following frame resulting height x width: " << height << "x" << width << "\n";

    // Enter get/send state, in which in 2 threads:
    // 1. Get frame, Convert frame, Send frame
    // 2. Get other persons frame, Print it
    // Threads are activated only if their existence is appropriate (? not anymore)

//    std::mutex mutex;
    ScreenManager screenManager(terminal.height, terminal.width);

    std::thread receiver = std::thread([&]() {
        logger << "Created receiving thread\n";
        while (true) {
            Coordinates print_coords(1, (terminal.width - width) / 2);
            std::unique_ptr<std::string> reply;
            if (network.GetMessage("video", reply) < 0) {
                break;
            }
            std::vector<std::vector<u_char>> matrix(height, std::vector<u_char>(width));
            if (reply->length() < width * height) {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        if (reply->length() > i * width + j) {
                            matrix[i][j] = (*reply)[i * width + j];
                        } else {
                            matrix[i][j] = ' ';
                        }
                    }
                }
            } else {
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        matrix[i][j] = (*reply)[i * width + j];
                    }
                }
            }
            screenManager.mvChunkPrint(print_coords, matrix);
            screenManager.refreshScreen(true);
        }
    });

    std::thread sender = std::thread([&]() {
        logger << "Created sending thread\n";
        std::pair<int, int> size = std::make_pair(companion_new_height, companion_new_width);
        size = ConvertSizeToPx(size);
        while (true) {
            std::string msg;
            if (camera.is_initialized) {
                camera.GetNewFrame();
                camera.PreprocessFrame(size);
                std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
                for (int i = 0; i < ascii_matrix.size(); ++i) {
                    for (int j = 0; j < ascii_matrix[0].size(); ++j) {
                        msg += ascii_matrix[i][j];
                    }
                }
            } else {
                msg = "here could be frame...";
                sleep(1);
            }
            if (network.SendMessage("video", msg) < 0) {
                break;
            }
        }
    });

    Chat chat({1 + height + 3, 0}, {terminal.height - height - 4, terminal.width}, screenManager);
    logger << "Created chat: " << terminal.height - height - 4 << "x" << terminal.width << "\n";


    const std::string my_msg_pref = "You > ";
    const std::string serv_msg_pref = "Server > ";
    const std::string ending_message = "The other person terminated chat.";
    std::atomic_bool chat_is_closed = false;

    std::thread rec([&]() {
        while (!chat_is_closed) {
            std::unique_ptr<std::string> message;
            if (network.GetMessage("chat", message) < 0) {
                chat_is_closed = true;
                *message = ending_message;
            }

            logger << "Got message from server: " << *message << "\n";
            *message = serv_msg_pref + *message;
            chat.addMessageToHistory(*message);
            chat.updateChat();
        }
    });

    std::thread sen([&]() {
        while (!chat_is_closed) {
            bool is_typing = true;
            while (is_typing) {
                is_typing = chat.processNewInput();
            }
            int control_code = chat.processMessage();
            if (control_code == -1) {
                chat_is_closed = true;
                network.TerminateConnection();
                return;
            }

            std::string message = WTOSTRING(chat.getMessage());
            logger << "Sending message: " << message << "\n";
            network.SendMessage("chat", message);

            chat.clearMessage();
            chat.updateChat();
        }
    });
    sen.join();
    rec.join();

    if (sender.joinable()) {
        sender.join();
    }
    if (receiver.joinable()) {
        receiver.join();
    }

    network.TerminateConnection();
}

void EnterChat(int option, Terminal &terminal, GUI &interface) {
    Network network({"tech", "chat"});
    if (network.Connect() < 0) {
        return;
    }
    if (option == 1) {  // host
        std::unique_ptr<std::string> pass_code;
        network.GetMessage("tech", pass_code); // get passcode from server
        logger << "Got passcode from server: " << *pass_code << "\n";

        network.GetMessage("tech", pass_code); // get confirmation from server that peer connected
        logger << "Connection with peer established\n";
    } else {            // client
        while (true) {
            auto coords = PrintInputMenu(terminal, interface.passcode_enter);
            std::string input = GetInputFromInputMenu(coords);
            logger << "Entered passcode: " << input << "\n";
            network.SendMessage("tech", input); // send entered passcode to server for confirmation

            std::unique_ptr<std::string> reply;
            network.GetMessage("tech", reply); // get server response
            int accepted = std::stoi(*reply);
            if (accepted) {
                logger << "Passcode accepted\n"
                          "Connection with peer established\n";
                break;
            }
            logger << "Passcode denied\n";
        }
    }

    ClearScreen();
    ScreenManager screenManager(terminal.height, terminal.width);
    Chat chat({terminal.height - 2, 0}, {terminal.height, terminal.width}, screenManager);


    const std::string my_msg_pref = "You > ";
    const std::string serv_msg_pref = "Server > ";
    const std::string ending_message = "The other person terminated chat.";
    std::atomic_bool chat_is_closed = false;

    std::thread receiver([&]() {
        while (!chat_is_closed) {
            std::unique_ptr<std::string> message;
            if (network.GetMessage("chat", message) < 0) {
                chat_is_closed = true;
                *message = ending_message;
            }

            logger << "Got message from server: " << *message << "\n";
            *message = serv_msg_pref + *message;
            chat.addMessageToHistory(*message);
            chat.updateChat();
        }
    });

    std::thread sender([&]() {
        while (!chat_is_closed) {
            bool is_typing = true;
            while (is_typing) {
                is_typing = chat.processNewInput();
            }
            int control_code = chat.processMessage();
            if (control_code == -1) {
                chat_is_closed = true;
                network.TerminateConnection();
                return;
            }

            std::string message = WTOSTRING(chat.getMessage());
            logger << "Sending message: " << message << "\n";
            network.SendMessage("chat", message);

            chat.clearMessage();
            chat.updateChat();
        }
    });
    sender.join();
    receiver.join();
//    network.TerminateConnection(logger);
}

// not working, needs printing
void SelfVideo(Terminal &terminal, WebCamera &camera) {
    camera.GetNewFrame();
    std::pair<int, int> ret_size = camera.GetFittedFrameSize(terminal.height, terminal.width);
    while (true) {
        timeout(1); // wait for keypress
        if (getch() != ERR) {
            break;
        }
        camera.GetNewFrame();
        camera.PreprocessFrame(ret_size);
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
        // PrintFrame(terminal, ascii_matrix);
    }
}

// not working, needs printing
void Benchmarking(Terminal &terminal, WebCamera &camera) {
    ScreenManager screenManager(terminal.height, terminal.width);
    Chat chat({0, 0}, {terminal.height, terminal.width}, screenManager);
    freopen("bench.txt","r",stdin);
    bool is_chatting = true;
    while (is_chatting) {
        bool is_typing = true;
        while (is_typing) {
            is_typing = chat.processNewInput();
        }
        int control_code = chat.processMessage();
        if (control_code == -1) {
            is_chatting = false;
        }

        std::string message = WTOSTRING(chat.getMessage());
        logger << "Got message: " << message << "\n";

        chat.clearMessage();
        chat.updateChat();
    }



//    camera.GetNewFrame();
//    std::pair<int, int> ret_size = camera.GetFittedFrameSize(terminal.height, terminal.width);
//    while (true) {
//        auto start1 = std::chrono::high_resolution_clock::now();
//        timeout(1); // wait for keypress
//        if (getch() != ERR) {
//            break;
//        }
//        auto start = std::chrono::high_resolution_clock::now();
//        camera.GetNewFrame();
//        auto end = std::chrono::high_resolution_clock::now();
//        logger << "Frame was taken in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
//               << "ms\n";
//        start = std::chrono::high_resolution_clock::now();
//
//        camera.PreprocessFrame(ret_size);
//        end = std::chrono::high_resolution_clock::now();
//        logger << "Frame was remapped in "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
//        start = std::chrono::high_resolution_clock::now();
//        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(camera.GetFrame());
//        end = std::chrono::high_resolution_clock::now();
//        logger << "Frame was converted in "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
//        start = std::chrono::high_resolution_clock::now();
//        //PrintFrame(terminal, ascii_matrix);
//        end = std::chrono::high_resolution_clock::now();
//        logger << "Frame was printed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
//               << "ms\n";
//        end = std::chrono::high_resolution_clock::now();
//        logger << "All cycle done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start1).count()
//               << "ms\n";
//    }
}


void Execute() {
    FILE *of = fopen("./cout_cerr.txt", "w");
    dup2(fileno(of), STDERR_FILENO);
    fclose(of);
//    std::ofstream file(
//            "./cout_cerr.txt"); // <---- somehow, if extracted into function, causes SIGSEGV 139 on call to std::cout
//
//    std::streambuf *old_cout_buf = std::cout.rdbuf(); // save cout/cerr old streambufs
//    std::streambuf *old_cerr_buf = std::cerr.rdbuf();
//
//    std::cout.rdbuf(file.rdbuf()); // Redirect cout/cerr to file
//    std::cerr.rdbuf(file.rdbuf()); // <----


    Terminal terminal;
    GUI interface;
    WebCamera camera;
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
                EnterVideoChat(option, terminal, camera, interface);
            }
        } else if (option == 2) { // unused
            coords = PrintMenu(terminal, interface.CS_choice);
            option = GetOption(coords, interface.CS_choice.num_of_options);

            if (option == 3) {
                continue;
            } else {
                EnterChat(option, terminal, interface);
            }
        } else if (option == 3) { // unused, for parameters
            Benchmarking(terminal, camera);
        } else if (option == 4) { // exit from application
            logger << "Exiting from the application\n";
            break;
        } else if (option == 5) { // for debug
            ClearScreen();

            ScreenManager screenManager(terminal.height, terminal.width);
            Chat chat({0, 0}, {terminal.height, terminal.width}, screenManager);

            bool is_chatting = true;
            while (is_chatting) {
                bool is_typing = true;
                while (is_typing) {
                    is_typing = chat.processNewInput();
                }
                int control_code = chat.processMessage();
                if (control_code == -1) {
                    is_chatting = false;
                }

                std::string message = WTOSTRING(chat.getMessage());
                logger << "Got message: " << message << "\n";

                chat.clearMessage();
                chat.updateChat();
            }
        }
    }

//    std::cout.rdbuf(old_cout_buf); // Restore cout/cerr streambufs before exit
//    std::cerr.rdbuf(old_cerr_buf);

}
