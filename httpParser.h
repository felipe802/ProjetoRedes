#ifndef HTTPPARSER_H
#define HTTPPARSER_H
#include <stdio.h>
#include <stdlib.h>

void processar_requisicao(int novo_socket);
void enviar_resposta_html(int socket);
void enviar_resposta_txt(int socket);

#endif