# Compilador
CC = clang

# Flags de compilação
CFLAGS = -Wall -Wextra -std=c11

# Nome do executável
TARGET = main

# Arquivo fonte
SOURCES = servidor.c

# Arquivos objeto (gerados automaticamente a partir dos .c)
OBJECTS = $(SOURCES:.c=.o)

# Regra padrão (quando digita apenas 'make')
all: 
	rm -f $(OBJECTS) $(TARGET)	
	@echo "Arquivos limpos!"
	$(CC) $(CFLAGS) -c $(SOURCES) -o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilação concluída! Exeecutando..."
	./$(TARGET)

# Indicar que essas regras não são arquivos
.PHONY: all 
