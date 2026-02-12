#ifndef HTTPPARSER_H
#define HTTPPARSER_H
#include <stdio.h>
#include <stdlib.h>

void processar_requisicao(int novo_socket);
void enviar_arquivo_generico(int socket, char arquivo[]);

#endif