CC = gcc
INCLUDES = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -fno-builtin $(INCLUDES)

SRC = \
src/input/validate.c \
src/input/normalize.c \
src/input/hash.c \
src/rules/integrity.c \
src/rules/device.c \
src/rules/failures.c \
src/engine/evaluate.c \
src/engine/version.c \
src/audit/log.c \
src/crypto/sha256.c \
src/security/vault_crypto.c 

OBJ = $(SRC:.c=.o)

all: risk_engine.o

risk_engine.o: $(OBJ)
	ld -r -o risk_engine.o $(OBJ)

clean:
	rm -f $(OBJ) risk_engine.o