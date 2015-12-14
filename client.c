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
    switch (c) {
        case 'a':
        case 'A':
        case 'e':
        case 'E':
        case 'i':
        case 'I':
        case 'o':
        case 'O':
        case 'u':
        case 'U':
            return 1;
        default:
            return 0;
    }
}


int main() {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("Error at WSASturtup!\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromAddrLen;
    char host_name[100];
    int port;
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

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    printf("Insert the name of the host you want to contact: ");
    fgets(host_name, sizeof(host_name), stdin);
    printf("Insert the number of port: ");
    scanf("%d", &port);
    host_name[strlen(host_name)-1] = '\0';
    struct hostent *host;
    host = gethostbyname(host_name);
    if (host == NULL) {
        fprintf(stderr, "gethostbyname() failed.\n");
        return 0;
    }
    struct in_addr newAddr;
    newAddr.s_addr = *(unsigned long *) host->h_addr_list[0];

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(inet_ntoa(newAddr));

    //INVIA PRIMO MESSAGGIO AL SERVER
    char *msg = "First message send to server from the client";
    msg_len = strlen(msg);
    if (sendto(sock, msg, msg_len, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != msg_len) {
        errorHandler("sendto() sent different number of bytes than expected!");
        closeConnection(sock);
        return 0;
    }

    //RICEZIONE DEL MESSAGGIO OK DAL SERVER E STAMPA A VIDEO
    fromAddrLen = sizeof(fromAddr);
    recvMsgSize = recvfrom(sock, buffer, DIM_MSG, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);
    if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(1);
    }
    buffer[recvMsgSize] = '\0';
    printf("%s\n", buffer);

    //LETTURA STRINGA IN INPUT
    char str1[DIM_MSG];
    printf("Insert a string: ");
    fflush(stdin);
    fgets(str1, DIM_MSG, stdin);
    if (strlen(str1) > DIM_MSG) {
        errorHandler("String too long");
    }

    //INVIA AL SERVER LA VOCALE E LA RICEVE IN MAIUSCOLO
    printf("Uppercase vowel received from server: ");
    int i = 0;
    while (str1[i] != '\0') {
        char c = str1[i];
        if (isVowel(c) == 1 || c == '\n') {
            if (isVowel(c) == 1) {
                //invia al server la vocale
                if (sendto(sock, &str1[i], 1, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 1) {
                    errorHandler("sendto() sent different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }

                //riceve dal server la vocale in maiuscolo
                fromAddrLen = sizeof(fromAddr);
                if ((recvMsgSize = recvfrom(sock, buffer, 1, 0, (struct sockaddr *) &fromAddr, &fromAddrLen)) <= 0) {
                    errorHandler("recvfrom() receive different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }
                if (serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
                    fprintf(stderr, "Error: received a packet from unknown source.\n");
                    exit(1);
                }
                buffer[recvMsgSize] = '\0';
                printf("%s, ", buffer);
            }
            else { //invia '\n' al server
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
    printf("\n");
    system("pause");
    return 0;
}