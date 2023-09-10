#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    const char* receiver_ip = "172.17.0.3"; // Replace with the actual IP address of the receiver container
    const int receiver_port = 5055; // Replace with the port the receiver is listening on
    const int packets_per_second = 100; // Desired packet rate

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in receiver_addr;
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(receiver_port);
    if (inet_pton(AF_INET, receiver_ip, &receiver_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    int packet_count = 0;
    clock_t start_time = clock();

    while (true) {
        const std::string data = "Packet " + std::to_string(packet_count);
        if (send(sock, data.c_str(), data.size(), 0) == -1) {
            perror("send");
            break;
        }
        ++packet_count;

        // Calculate packets per second
        clock_t elapsed_time = clock() - start_time;
        if (elapsed_time >= CLOCKS_PER_SEC) {
            double packets_per_sec = static_cast<double>(packet_count) / (elapsed_time / static_cast<double>(CLOCKS_PER_SEC));
            std::cout << "Packets per second: " << packets_per_sec << std::endl;
            packet_count = 0;
            start_time = clock();
        }

        // Sleep to achieve the desired packet rate (100 packets per second)
        usleep(1000000 / packets_per_second);
    }

    close(sock);
    return 0;
}