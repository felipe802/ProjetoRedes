# Compilador
CC = clang

# Flags de compilação
CFLAGS = -Wall -Wextra -std=c11

# Nome do executável
TARGET = main

# Arquivos fonte
SOURCES = servidor.c httpParser.c
HEADERS = httpParser.h

# Arquivos objeto (gerados automaticamente a partir dos .c)
OBJECTS = $(SOURCES:.c=.o)

# Regra padrão (quando digita apenas 'make')
all:
	@rm -f $(OBJECTS) $(TARGET)
	@echo "Arquivos limpos!"
	$(CC) $(CFLAGS) -c servidor.c -o servidor.o
	$(CC) $(CFLAGS) -c httpParser.c -o httpParser.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilação concluída! Executando..."
	./$(TARGET)

# Indicar que essas regras não são arquivos
.PHONY: all