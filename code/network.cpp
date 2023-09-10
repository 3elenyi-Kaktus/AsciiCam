#include "network.h"

Network::Network(const std::vector<std::string> &IDs) : send_buf(BUF_SIZE), recv_buf(BUF_SIZE) {
    for (const std::string &id: IDs) {
        data_packets[id];
        locks[id];
        availability[id];
    }
    send_buf.reserve(BUF_SIZE);
    //recv_buf.reserve(BUF_SIZE);
}

int Network::Connect(Logger &logger) {
    char port_no[] = "33333\0";
    char ipaddr[] = "localhost\0";

    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    struct addrinfo *result;
    int gai_error = getaddrinfo(ipaddr, port_no, &hints, &result);
    if (gai_error) {
        logger << gai_strerror(gai_error) << "\n";
        return -1;
    }
    for (struct addrinfo *a_info = result; a_info; a_info = a_info->ai_next) {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            logger << "ERROR on opening socket\nTrying again\n";
            continue;
        }
        if (connect(sock_fd, a_info->ai_addr, a_info->ai_addrlen) < 0) {
            close(sock_fd);
            sock_fd = -1;
            logger << "ERROR on connecting\nTrying again\n";
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (sock_fd < 0) {
        logger << "Couldn't connect to any of available TCP/Ip4 addresses\n";
        return -1;
    }
    logger << "Connected to server successfully\n";
    poller = std::thread([&]() {
        while (true) {
            logger << "Poller trying to get message\n";
            ssize_t n = recv(sock_fd, &(recv_buf[0]), BUF_SIZE, MSG_WAITALL);
            if (n <= 0) {
                logger << "ERROR on reading from socket\n";
                for (auto &[id, queue]: data_packets) {
                    std::unique_lock<std::mutex> mtx(locks[id]);
                    queue.push_back(std::make_unique<std::string>("!stop"));
                    if (queue.size() == 1) {
                        availability[id].notify_one();
                    }
                }
                return;
            }
//            logger << "Poller got message \"" << recv_buf << "\", start decode\n";
            std::string packet_id;
            int pos = 0;
            while (recv_buf[pos] != ' ') {
                packet_id += recv_buf[pos];
                ++pos;
            }
//            if (!data_packets.contains(packet_id)) {
//                data_packets[packet_id] = {};
//            }
            std::unique_lock<std::mutex> mtx(locks[packet_id]);
            data_packets[packet_id].push_back(std::make_unique<std::string>(&(recv_buf[0]) + pos + 1));
            logger << "Added packet with ID \"" << packet_id << "\", with msg \"" << *data_packets[packet_id].back()
                   << "\"\n";
            if (data_packets[packet_id].size() == 1) {
                availability[packet_id].notify_one();
            }
        }
    });
    poller.detach();
    logger << "Created polling thread\n";
    return 0;
}

// return 0 upon successfull request
// return -1 if reached connection termination point
int Network::GetMessage(const std::string &id, std::unique_ptr<std::string> &dest, Logger &logger) {
    logger << "Message requested for \"" << id << "\", getting mutex\n";
    std::unique_lock<std::mutex> mtx(locks[id]);
    while (data_packets[id].empty()) {
        availability[id].wait(mtx);
    }
    logger << "Message retrieved: \"" << *data_packets[id].front() << "\"\n";
    dest = std::move(data_packets[id].front());
    data_packets[id].pop_front();
    if (*dest == "!stop") {
        return -1;
    }
    return 0;
}

int Network::SendMessage(const std::string &id, const std::string &message, Logger &logger) {
    std::copy(id.begin(), id.end(), send_buf.begin());
    send_buf[id.length()] = ' ';
    std::copy(message.begin(), message.end(), send_buf.begin() + id.length() + 1);
    *(send_buf.begin() + id.length() + message.length() + 1) = '\0';
    logger << "Sending: \"" << message << "\"\n";
    ssize_t n = send(sock_fd, &(send_buf[0]), BUF_SIZE, MSG_NOSIGNAL);
    if (n <= 0) {
        logger << "ERROR on writing to socket\n";
        return -1;
    }
    return 0;
}

void Network::TerminateConnection(Logger &logger) {
    logger << "Connection with server terminated\n";
    shutdown(sock_fd, SHUT_RDWR);
    logger << "Polling ended\n";
}

Network::~Network() {
    close(sock_fd);
}
