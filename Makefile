# Compilador e flags
CC = gcc
CFLAGS = -Wall -g -Iinclude 

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN = exe

# Arquivos fonte e objeto
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regra principal
all: $(BIN)

# Linkagem
$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ -lm

# Compilação de cada .c para .o no diretório obj
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Criação do diretório obj, se necessário
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(BIN)
