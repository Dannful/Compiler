# Grupo C
# Alunos:
# Gustavo Sleman Lenz - 00290394
# Vinicius Daniel Spadotto - 00341554

CC = gcc
INCLUDES = obj include
CFLAGS = -lfl -g -Wall $(foreach dir,$(INCLUDES),-I$(dir))
CFLAGS_LEXYY = -lfl -g $(foreach dir,$(INCLUDES),-I$(dir))

SRC_DIR = src
OBJ_DIR = obj

FLEX_FILE = $(SRC_DIR)/scanner.l
BISON_FILE = $(SRC_DIR)/parser.y

TARGET = etapa5

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parser.tab.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)/parser.tab.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lex.yy.c: $(FLEX_FILE)
	@mkdir -p $(OBJ_DIR)
	flex -o $@ $<

$(OBJ_DIR)/lex.yy.o: $(OBJ_DIR)/lex.yy.c $(OBJ_DIR)/parser.tab.h
	$(CC) $(CFLAGS_LEXYY) -c $< -o $@

$(OBJ_DIR)/parser.tab.c $(OBJ_DIR)/parser.tab.h: $(BISON_FILE)
	@mkdir -p $(OBJ_DIR)
	bison -d -o $(OBJ_DIR)/parser.tab.c $<

$(OBJ_DIR)/parser.tab.o: $(OBJ_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean
