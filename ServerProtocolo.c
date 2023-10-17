#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define MESSAGESIZE 1024
#define MAXSOCKETS 10

typedef struct Node {
    SOCKET socket;
    struct Node* next;
} Node;

Node* connections = NULL;
int qtdsockets = 0;

bool isValidCPF(const char* cpf) {
    int i;
    int j = 10;
    int sum1 = 0, sum2 = 0;

    for (i = 0; i < 9; i++) {
        if (!isdigit(cpf[i])) {
            return false;
        }
        sum1 += (cpf[i] - '0') * j;
        j--;
    }

    sum1 %= 11;
    if (sum1 < 2) {
        sum1 = 0;
    } else {
        sum1 = 11 - sum1;
    }

    j = 11;
    for (i = 0; i < 10; i++) {
        sum2 += (cpf[i] - '0') * j;
        j--;
    }

    sum2 %= 11;
    if (sum2 < 2) {
        sum2 = 0;
    } else {
        sum2 = 11 - sum2;
    }

    return (cpf[9] - '0' == sum1) && (cpf[10] - '0' == sum2);
}

bool isValidCNPJ(const char* cnpj) {
    int i, j;
    int sum1 = 0, sum2 = 0;
    int digit;

    if (strlen(cnpj) != 14) {
        return false;
    }

    for (i = 0, j = 5; i < 12; i++, j--) {
        if (!isdigit(cnpj[i])) {
            return false;
        }
        digit = cnpj[i] - '0';
        sum1 += digit * j;
        if (j == 2) {
            j = 9;
        }
    }

    sum1 %= 11;
    sum1 = (sum1 < 2) ? 0 : (11 - sum1);

    for (i = 0, j = 6; i < 13; i++, j--) {
        if (!isdigit(cnpj[i])) {
            return false;
        }
        digit = cnpj[i] - '0';
        sum2 += digit * j;
        if (j == 2) {
            j = 9;
        }
    }

    sum2 %= 11;
    sum2 = (sum2 < 2) ? 0 : (11 - sum2);

    return (cnpj[12] - '0' == sum1) && (cnpj[13] - '0' == sum2);
}


void addConnection(SOCKET new_socket) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->socket = new_socket;
    newNode->next = connections;
    connections = newNode;
}

void removeConnection(SOCKET socketToRemove) {
    Node* current = connections;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->socket == socketToRemove) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                connections = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
}

void handleRequest(SOCKET client_socket) {
    int len;
    char request[MESSAGESIZE];
    char response[MESSAGESIZE];
    char number[MESSAGESIZE];
    char format[MESSAGESIZE];

    while (1) {
        request[0] = '\0';
        len = recv(client_socket, request, MESSAGESIZE, 0);
        if (len > 0) {
            sscanf(request, "%[^,],%s", number, format);

            int responseCode = 3;

            if (strlen(number) == 11 && strcmp(format, "0") == 0 && isValidCPF(number)) {
                responseCode = 0;
            } else if (strlen(number) == 14 && strcmp(format, "1") == 0 && isValidCNPJ(number)) {
                responseCode = 0;
            } else if (strlen(number) != 11 && strlen(number) != 14) {
                responseCode = 2;
            }

            sprintf(response, "%s\nResposta: %d\n", number, responseCode);
            send(client_socket, response, strlen(response), 0);
            closesocket(client_socket);
            removeConnection(client_socket);
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server, client;
    int c;
    char errormessage[MESSAGESIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("\nFalha na inicializacao da biblioteca Winsock: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\nNao e possivel inicializar o socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("\nNao e possivel construir o socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    listen(s, 3);
    c = sizeof(struct sockaddr_in);
    printf("*** SERVER ***\n\nAguardando conexoes...\n\n");

    while (1) {
        SOCKET new_socket = accept(s, (struct sockaddr*)&client, &c);
        if (new_socket == INVALID_SOCKET) {
            printf("\nConexao nao aceita. Codigo de erro: %d", WSAGetLastError());
        } else {
            puts("\nConexao aceita.");
            printf("\nDados do cliente - IP: %s -  Porta: %d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));

            if (qtdsockets < MAXSOCKETS) {
                addConnection(new_socket);
                _beginthread(handleRequest, 0, new_socket);
                qtdsockets++;
            } else {
                puts("\nNumero maximo de conexoes excedido!");
                strcpy(errormessage, "nToo Many Requests");
                send(new_socket, errormessage, strlen(errormessage) + 1, 0);
                closesocket(new_socket);
            }
        }
    }

    Node* current = connections;
    while (current != NULL) {
        Node* next = current->next;
        closesocket(current->socket);
        free(current);
        current = next;
    }
    closesocket(s);
    WSACleanup();
}
