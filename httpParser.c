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
    // GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n ...
    // O que vier depois do cabeçalho é o corpo, útil para o POST
    char metodo[10], caminho[256], versao[20];
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

    sscanf(buffer, "%9s %255s %19s", metodo, caminho, versao);
    printf("O cliente quer usar o método: %s\n", metodo);
    printf("O arquivo solicitado eh: %s\n", caminho);
    char *corpo = strstr(buffer, "\r\n\r\n");
    if (corpo) {
        corpo += 4; // Pula os quatro caracteres (\r\n\r\n) para chegar no início dos dados
        printf("Dados do corpo recebidos: %s\n", corpo);
    }

    if (strcmp(metodo, "GET") == 0) {
        char arquivo_alvo[256];
        if (strcmp(caminho, "/") == 0) { // Index.html por padrão 
            strcpy(arquivo_alvo, "index.html");
        }
        else {
            strcpy(arquivo_alvo, caminho + 1); // O "+1" pula a barra inicial
        }
        enviar_arquivo_generico(novo_socket, arquivo_alvo);
    }
    else if (strcmp(metodo, "POST") == 0) {
        printf("Recebi um POST para o caminho %s\n", caminho);
        // Verifica se realmente chegou algum dado no corpo da requisicao
        if (corpo != NULL && strlen(corpo) > 0) {
            printf("Dados recebidos do cliente: %s\n", corpo);
        }
        else {
            printf("POST recebido, mas o corpo estava vazio.\n");
        }

        // 2. montando a resposta HTTP para o cliente nao ficar travado
        char resposta[512];
        char *mensagem_retorno = "{\"status\": \"sucesso\", \"mensagem\": \"POST recebido pelo servidor C!\"}";

        snprintf(resposta, sizeof(resposta),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"    // Devolvendo um JSON
            "Content-Length: %lu\r\n"
            "Connection: close\r\n"                 // Avisa que vamos fechar a conexão
            "\r\n"
            "%s",                                   // O corpo da resposta entra aqui
            strlen(mensagem_retorno), mensagem_retorno);

        // Enviando a resposta de volta pelo socket
        int bytes_enviados = send(novo_socket, resposta, strlen(resposta), 0);
        if (bytes_enviados < 0) perror("Erro ao enviar resposta do POST");
        else printf("Resposta do POST enviada com sucesso.\n");
    }

}


void enviar_arquivo_generico(int socket_cliente, char *arquivo) {
    FILE *f = fopen(arquivo, "rb"); // rb para Unix não corromper outros tipos
    if (f == NULL) {
        perror("Erro ao abrir arquivo");
        char *erro = "HTTP://1.1 404 Not Found\r\nContent-Type: text-html\r\n\r\n<h1>404 - Nao encontrado</h1>";
        send(socket_cliente, erro, strlen(erro), 0);
        return;
    }

    fseek(f, 0, SEEK_END);      // Envia o cursor para o final do doc
    size_t tamanho = ftell(f);  // Quantos bytes ele andou
    fseek(f, 0, SEEK_SET);      // Traz o cursor pro começo

    char *conteudo = (char*) malloc(tamanho);
    fread(conteudo, 1, tamanho, f); // Escreve o que tem em f no conteudo

    char cabecalho[512]; // Montar o cabecalho HTTP dinamicamente
    // Esta função formata a string do cabeçalho + corpo da resposta
    // Escreve no buffer com segurança do tamanho
    snprintf(cabecalho, sizeof(cabecalho),
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
    else printf("Conteudo enviado com êxito: %d bytes.\n", bytes_conteudo);

    free(conteudo);
    fclose(f);    
}