#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // Define a interface de sockets. Aqui estão as funções e a conexão feita na rede
#include <netinet/in.h> // Define as estruturas de endereçamento da internet (IPv4/IPv6)
#include <unistd.h> // Necessária para a função close()

#define PORTA 8050

int main() {
    // 1. Criar socket e definir a estrutura dele.
    int serv_file_desc;
    if ((serv_file_desc = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket");
        return (EXIT_FAILURE);
    }
    else printf("\nSocket criado: %d\n", serv_file_desc);

    struct sockaddr_in6 ender_serv;
    memset(&ender_serv, 0, sizeof(ender_serv));
    ender_serv.sin6_family = AF_INET6; // IPv6
    ender_serv.sin6_addr = in6addr_any;
    ender_serv.sin6_port = htons(PORTA); // Inverter a sequência dos bytes
    
    // 2. "Assigning a name to a socket". Bind->Listen->Accept
    if (bind(serv_file_desc, (struct sockaddr *)&ender_serv, sizeof(ender_serv)) < 0) {
        perror("Erro no bind");
        close(serv_file_desc);
        return(EXIT_FAILURE);
    }
    else printf("Bind feito com sucesso!\n");

    listen(serv_file_desc, 3);
    printf("Servidor iniciado. Aguardando conexão na porta %d...\n", PORTA);

    while (1) {
        struct sockaddr_in cliente;
        socklen_t c = sizeof(cliente);
        int novo_socket = accept(serv_file_desc, (struct sockaddr *)&cliente, &c);
        if (novo_socket < 0) {
            perror("Erro no accept");
            close(serv_file_desc);
            return (EXIT_FAILURE);
        }
        else printf("Novo socket criado: %d\n", novo_socket);

        // 3. Comunicação com o cliente
        char buffer[2048] = {};
        int bytes = recv(novo_socket, buffer, 1024, 0);
        if (bytes < 0) {
            perror("Erro na leitura do cliente");
            close(novo_socket);
            close(serv_file_desc);
            return(EXIT_FAILURE);
        }
        else if (bytes == 0) printf("Cliente desconectou.\n");
        else {
            buffer[bytes] = '\0'; // Coloca fim na string
            printf("Recebidos com êxito: %d bytes.\n", bytes);
        }
        // O curl precisa de pelo menos o Status Line e uma linha vazia para montar a HTTP básica
        char *mensagem = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 14\r\n"
                        "\r\n"
                        "Hello, world!\n";
        int bytes_enviados = send(novo_socket, mensagem, strlen(mensagem), 0);
        if (bytes_enviados < 0) {
            perror("Erro ao enviar feedback");
            close(novo_socket);
            close(serv_file_desc);
            return(EXIT_FAILURE);
        }
        else printf("Enviados com êxito: %d bytes.\n", bytes_enviados);
        close(novo_socket);
        printf("Novo socket encerrado.\n\n");
    }

    close(serv_file_desc);
    printf("Primeiro socket encerrado.\n");
    printf("Servidor encerrado com sucesso!\n");
    return(EXIT_SUCCESS);
}