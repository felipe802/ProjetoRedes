#include <sys/types.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>     
#include "httpParser.h"

void processar_requisicao(int novo_socket) {
    // Recebendo algo como:
    // GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n
    char metodo[10];
    char caminho[256];
    char versao[20];

    char buffer[2048] = {};
    memset(buffer, 0, sizeof(buffer));
    int bytes_recebidos = recv(novo_socket, buffer, sizeof(buffer)-1, 0);

    if (bytes_recebidos < 0) {
        perror("Erro na leitura do cliente");
        close(novo_socket);
        return;
    }
    else if (bytes_recebidos == 0) printf("Cliente desconectou.\n");
    else {
        buffer[bytes_recebidos] = '\0'; // Coloca fim na string
        printf("Recebidos com êxito: %d bytes.\n", bytes_recebidos);
    }

    sscanf(buffer, "%s %s %s", metodo, caminho, versao);
    printf("O cliente quer usar o método: %s\n", metodo);
    printf("O arquivo solicitado eh: %s\n", caminho);

    char *corpo = strstr(buffer, "\r\n\r\n");
    if (corpo) {
        corpo += 4; // Pula os quatro caracteres (\r\n\r\n) para chegar no início dos dados
        printf("Dados do corpo recebidos: %s\n", corpo); 
    }

    if (strcmp(metodo, "GET") == 0) {
        // Aqui é implementada a lógica de abrir um arquivo
        if (strcmp(caminho, "/") == 0) {
            // Se pedir a raiz, é entregue o index.html
            enviar_resposta_html(novo_socket);
        }
        // else if (strcmp(caminho, "") == 0) {
        //     enviar_resposta_txt(novo_socket);
        // }
        else {
            printf("trabalhando nisso\n");
        }
    }
    else if (strcmp(metodo, "POST") == 0) {
        // Lógica para salvar dados...
        printf("Recebi um POST para o caminho %s\n", caminho);
    }
}


void enviar_resposta_html(int socket_cliente) {
    FILE *f = fopen("index.html", "rb");
    if (f == NULL) {
        perror("Erro ao abrir arquivo");
        char *erro = "HTTP://1.1 404 Not Found\r\n\r\n<h1>Pagina nao encontrada</h1>";
        send(socket_cliente, erro, strlen(erro), 0);
        return;
    }

    fseek(f, 0, SEEK_END);      // Envia o cursor para o final do doc
    size_t tamanho = ftell(f);  // Quantos bytes ele andou
    fseek(f, 0, SEEK_SET);      // Traz o cursos pro começo

    char *conteudo = (char*) malloc(tamanho);
    fread(conteudo, 1, tamanho, f); // Escreve o que tem em f no conteudo

    char cabecalho[512]; // Montar o cabecalho HTTP dinamicamente
    sprintf(cabecalho, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n" 
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n", tamanho);   


    int bytes_cabecalho = send(socket_cliente, cabecalho, strlen(cabecalho), 0);
    if (bytes_cabecalho < 0) {
        perror("Erro ao enviar cabecalho");
        close(socket_cliente);
        return;
    }
    else printf("Cabecalho enviado com êxito: %d bytes.\n", bytes_cabecalho);

    int bytes_conteudo = send(socket_cliente, conteudo, tamanho, 0);
    if (bytes_conteudo < 0) {
        perror("Erro ao enviar conteudo");
        close(socket_cliente);
        return;     
    }
    else printf("Conteudo enviado com êxito: %d bytes.\n", bytes_cabecalho);

    free(conteudo);
    fclose(f);    
}