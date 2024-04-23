#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

#define MAX_BUFFER_SIZE 256

int main() {
    int sockfd, newsockfd, portno = 12345;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Ошибка открытия сокета" << std::endl;
        return -1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Ошибка привязки" << std::endl;
        return -1;
    }

    while (true) {
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        if (newsockfd < 0) {
            std::cerr << "Ошибка на accept" << std::endl;
            return -1;
        }

        bzero(buffer, MAX_BUFFER_SIZE);
        ssize_t n = read(newsockfd, buffer, MAX_BUFFER_SIZE - 1);
        if (n < 0) {
            std::cerr << "Ошибка чтения из сокета" << std::endl;
            return -1;
        }

        std::cout << "Получено сообщение: " << buffer << std::endl;
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
