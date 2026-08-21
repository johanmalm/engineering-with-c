CC = gcc
RM = rm -f
MAKEFLAGS += --no-print-directory
QUIET_CC   = @echo '     CC    '$@;
QUIET_LINK = @echo '     LINK  '$@;

CFLAGS = -g -Wall -Wextra -std=c11 -pedantic -fanalyzer -flto
CFLAGS += -fsanitize=address,undefined -fno-common -fno-omit-frame-pointer -rdynamic
CFLAGS += -fanalyzer
CFLAGS += -Wno-unused-parameter
CFLAGS += `pkg-config --cflags glib-2.0`
LDFLAGS += `pkg-config --libs glib-2.0`

PROGS = \
	list-singly-linked \
	remove-spaces \
	array-fixed-size-strings \
	array-fixed-size-structs \
	array-dynamic-structs \
	array-dynamic-strings-glib \
	generate-readme

HELPER = \
	util.o \
	parse-key-value-pair.o \
	string-helpers.o

SRC  = $(wildcard *.c)
OBJ  = $(SRC:.c=.o)

all: $(PROGS)

$(PROGS): % : %.o $(HELPER)
	$(QUIET_LINK)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f $(PROGS) $(OBJ)
