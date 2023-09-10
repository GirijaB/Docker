#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <string>


// Function to continuously receive data from the sender
void receiveData(int senderConnection) {
    char buffer[1024];
    int bytesRead;
    int packetCount = 0;  // Counter for received packets

    while ((bytesRead = recv(senderConnection, buffer, sizeof(buffer), 0)) > 0) {
        // Create a unique filename based on packetCount and the current timestamp
        time_t timestamp = time(nullptr);
        std::stringstream filenameStream;
        filenameStream << "received_packet_" << packetCount << "_"
                       << timestamp << ".txt";
        std::string filename = filenameStream.str();

        // Open a new file for each received packet
        std::ofstream outputFile(filename, std::ios::binary);
        if (!outputFile.is_open()) {
            std::cerr << "Error opening the output file." << std::endl;
            close(senderConnection);
            return;
        }

        // Write the received data to the output file
        outputFile.write(buffer, bytesRead);

        // Flush and close the output file
        outputFile.flush();
        outputFile.close();

        packetCount++;
    }

    // Close the sender connection
    close(senderConnection);
}

int main() {
    // Define the ports for receiving and storing data
    int receivePort = 5055;  // Port for receiving data
    int storePort = 5056;    // Port for storing data

    // Create a socket for receiving data
    int receiveSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (receiveSocket == -1) {
        std::cerr << "Error creating receive socket." << std::endl;
        return 1;
    }

    // Define the address and port for receiving data
    struct sockaddr_in receiveAddr;
    receiveAddr.sin_family = AF_INET;
    receiveAddr.sin_addr.s_addr = INADDR_ANY;
    receiveAddr.sin_port = htons(receivePort);

    // Bind the receive socket to the address and port
    if (bind(receiveSocket, (struct sockaddr *)&receiveAddr, sizeof(receiveAddr)) == -1) {
        std::cerr << "Error binding receive socket." << std::endl;
        close(receiveSocket);
        return 1;
    }

    // Listen for incoming connections on the receive socket
    if (listen(receiveSocket, 1) == -1) {
        std::cerr << "Error listening on receive socket." << std::endl;
        close(receiveSocket);
        return 1;
    }

    // Create a unique filename based on the current timestamp
    time_t timestamp = time(nullptr);
    char filename[128];
    strftime(filename, sizeof(filename), "/app/data/received_data_%Y%m%d%H%M%S.txt", localtime(&timestamp));

    // Open the unique file for storing received data
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the output file." << std::endl;
        close(receiveSocket);
        return 1;
    }

    // Accept a connection from the sender to receive data
    int senderConnection = accept(receiveSocket, nullptr, nullptr);
    if (senderConnection == -1) {
        std::cerr << "Error accepting sender connection." << std::endl;
        close(receiveSocket);
        return 1;
    }

    // Create a thread to receive data and continuously write to new files
    std::thread receiveThread(receiveData, senderConnection);

    // Join the receive thread
    receiveThread.join();


    // Close the sender connection, receive socket, and file
    close(senderConnection);
    close(receiveSocket);
    outputFile.close(); 

    std::cout << "Data has been received and stored in " << filename << "." << std::endl;

    return 0;
}