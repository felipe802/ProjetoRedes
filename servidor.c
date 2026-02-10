#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // Define a interface de sockets. Aqui estão as funções e a conexão feita na rede
#include <netinet/in.h> // Define as estruturas de endereçamento da internet (IPv4/IPv6)

#define PORTA 8080

int main() {
    // 1. Criar socket e definir a estrutura dele.
    int serv_file_desc;
    if ((serv_file_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erro ao criar socket!\n");
        return (EXIT_FAILURE);
    }
    struct sockaddr_in ender_serv;
    memset(&ender_serv, 0, sizeof(ender_serv));
    ender_serv.sin_family = AF_INET; // IPv4
    ender_serv.sin_addr.s_addr = INADDR_ANY;
    ender_serv.sin_port = htons(PORTA); // Inverter a sequência dos bytes
    
    close(serv_file_desc);
    return 0;
}