#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // Define a interface de sockets. Aqui estão as funções e a conexão feita na rede
#include <netinet/in.h> // Define as estruturas de endereçamento da internet (IPv4/IPv6)
#include <unistd.h> // Necessária para a função close()

#define PORTA 8080

int main() {
    // 1. Criar socket e definir a estrutura dele.
    int serv_file_desc;
    if ((serv_file_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket!\n");
        return (EXIT_FAILURE);
    }
    else printf("Socket criado: %d\n", serv_file_desc);
    struct sockaddr_in ender_serv;
    memset(&ender_serv, 0, sizeof(ender_serv));
    ender_serv.sin_family = AF_INET; // IPv4
    ender_serv.sin_addr.s_addr = INADDR_ANY;
    ender_serv.sin_port = htons(PORTA); // Inverter a sequência dos bytes
    
    // 2. "Assigning a name to a socket". Bind->Listen->Accept
    if (bind(serv_file_desc, (struct sockaddr *)&ender_serv, sizeof(ender_serv)) < 0) {
        perror("Erro no bind.\n");
        return(EXIT_FAILURE);
    }
    else printf("Bind feito com sucesso!\n");

    listen(serv_file_desc, 3);
    printf("Servidor iniciado. Aguardando conexão na porta 8080...\n");

    struct sockaddr_in cliente;
    socklen_t c = sizeof(struct sockaddr);
    int novo_socket = accept(serv_file_desc, (struct sockaddr *)&cliente, &c);
    if (novo_socket < 0) {
        perror("Erro no accept.\n");
        return (EXIT_FAILURE);
    }
    else printf("Novo socket criado: %d\n", novo_socket);

    close(novo_socket);
    close(serv_file_desc);
    return(EXIT_SUCCESS);
}