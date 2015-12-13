//
// Created by lateg on 12/12/2015.
//

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h> /* for memset() */

#define DIM_MSG 255
#define PORT 18000

void errorHandler(char *errorMessage) {
    printf(errorMessage);
}

void clearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

void closeConnection(int socket) {
    closesocket(socket);
    clearWinSock();
}

int isVowel(char c) {
    switch(c) {
        case 'a': case 'A':
        case 'e': case 'E':
        case 'i': case 'I':
        case 'o': case 'O':
        case 'u': case 'U':
            return 1;
        default:
            return 0;
    }
}


int main() {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
    if (iResult != 0) {
        printf ("Error at WSASturtup!\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromAddrLen;
    char host_name[DIM_MSG];
    unsigned int port;
    int msg_len;  //dimensione del messaggio
    char buffer[DIM_MSG];  //buffer per la ricezione
    int recvMsgSize;  //dimensione del messaggio ricevuto

    unsigned int fromSize;

    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        errorHandler("socket() failed!");
        closeConnection(sock);
        return 0;
    }

    //printf("Insert the name of the host you want to contact: ");
    //scanf("%s", host_name);
   // printf("Insert the number of port: ");
    //scanf("%d", &port);

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(18000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //INVIA PRIMO MESSAGGIO AL SERVER
    char *msg = "First message send to server";
    msg_len = strlen(msg);
    if (sendto(sock, msg, msg_len, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != msg_len) {
        errorHandler("sendto() sent different number of bytes than expected!");
        closeConnection(sock);
        return 0;
    }

    //RICEVE OK DAL SERVER
    fromAddrLen = sizeof(fromAddr);
    recvMsgSize = recvfrom(sock, buffer, DIM_MSG, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);
    if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(1);
    }
    buffer[recvMsgSize] = '\0';
    printf("%s\n", buffer);

    //PRENDE STRINGA IN INPUT
    char str1[DIM_MSG];
    printf("Insert a string: ");
    fgets(str1, DIM_MSG, stdin);

    printf("Uppercase vowel received from server: ");
    int i = 0;
    while (str1[i] != '\0') {
        char c = str1[i];
        if(isVowel(c) == 1 || c == '\n') {
            if(isVowel(c) == 1) {
                if (sendto(sock, &str1[i], 1, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 1) {
                    errorHandler("sendto() sent different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }


                fromAddrLen = sizeof(fromAddr);
                if ((recvMsgSize = recvfrom(sock, buffer, 1, 0, (struct sockaddr *) &fromAddr, &fromAddrLen)) <= 0) {
                    errorHandler("recvfrom() receive different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }
                buffer[recvMsgSize] = '\0';
                printf("%s, ", buffer);
            }
            else {
                if (sendto(sock, &str1[i], 1, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 1) {
                    errorHandler("sendto() sent different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }
            }
        }
        i++;
    }

    closeConnection(sock);
    system("pause");
    return 0;
}