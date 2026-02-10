/* 
socket() cria o endpoint de comunicacao
bind() associa o socket a uma porta especifica
listen() coloca o socket em modo de espera por conexoes
accept() retira uma conexao da fila e cria um novo socket para essa conversa
read()/write() troca de dados (onde entra o protocolo HTTP)
close() encerra a conexao
*/