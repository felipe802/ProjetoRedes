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
all: $(TARGET)

# Como criar o executável
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilação concluída! Execute com: ./$(TARGET)"

# Como compilar arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Arquivos limpos!"

# Recompilar tudo do zero
rebuild: clean all

# Executar o servidor
run: $(TARGET)
	./$(TARGET)

# Indicar que essas regras não são arquivos
.PHONY: all clean rebuild run

# make            Compila o projeto (cria o executável 'servidor')
# make run        Compila e executa automaticamente
# make clean      Remove arquivos compilados
# make rebuild    Limpa tudo e recompila do zero