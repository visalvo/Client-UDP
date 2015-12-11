//
// Created by lateg on 12/12/2015.
//

#if defined WIN32
#include <winsock.h>
#else #define closesocket close

#include <sys/socket.h>
#include <arpa/inet h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#include <stdio.h>
#include <string.h> /* for memset() */

#define ECHOMAX 255
#define PORT 18000

void errorHandler(char *errorMessage) {
    printf(errorMessage);
}

void ClearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}


int main() {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
    if (iResult != 0) {
        printf ("error at WSASturtup\n"); return 0;
    }
#endif

    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromSize;
    char echoString[ECHOMAX];
    char echoBuffer[ECHOMAX];
    char echoBuffer[ECHOMAX];
    int echoStringLen;
    int respStringLen;

    printf("Inserisci la stringa echo da inviare al server\n");
    scanf("%s", echoString);
    if ((echoStringLen = strlen(echoString)) > ECHOMAX)
        errorHandler("echo word too long");


    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        errorHandler("socket() failed");

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = PF_INET;
    echoServAddr.sin_port = htons(PORT);
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // INVIO DELLA STRINGA ECHO AL SERVER
    if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) !=
        echoStringLen)
        errorHandler("sendto() sent different number of bytes than expected");

    // RITORNO DELLA STRINGA ECHO
    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(1);
    }

    echoBuffer[respStringLen] = '\0';
    printf("Received: %s\n", echoBuffer);

    closesocket(sock);
    ClearWinSock();
    system("pause");
    return 0;
}