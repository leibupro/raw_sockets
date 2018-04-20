SHELL := /bin/bash --login

IF0 := alice
IF1 := bob
IF2 := foo
IF3 := bar

IF0_L2 := 02:DE:AD:BE:EF:01
IF1_L2 := 02:DE:AD:BE:EF:02
IF2_L2 := 02:DE:AD:BE:EF:03
IF3_L2 := 02:DE:AD:BE:EF:04

CC := gcc
LD := $(CC)

CF := -g -O0 -std=gnu89 -Wall -Wextra -Werror
LF := 

BINDIR := ./bin
OBJDIR := ./obj
SRCDIR := ./src/c
INCDIR := ./include

SRC := $(SRCDIR)/mitm.c

OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))

BINARY := $(BINDIR)/mitm

VPATH := $(SRCDIR)


.PHONY: all setup teardown clean


all: $(BINARY)


$(OBJDIR):
	mkdir $@


$(BINDIR):
	mkdir $@


$(BINARY): $(OBJ) $(BINDIR)
	$(LD) $(LF) $< -o $@


$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) $(CF) -I$(INCDIR) -c $< -o $@


# setting up veth ethernet interfaces
setup:
	@sudo ip link add $(IF0) type veth peer name $(IF2)
	@sudo ip link add $(IF1) type veth peer name $(IF3)
	@sudo ip link set dev $(IF0) address $(IF0_L2)
	@sudo ip link set dev $(IF1) address $(IF1_L2)
	@sudo ip link set dev $(IF2) address $(IF2_L2)
	@sudo ip link set dev $(IF3) address $(IF3_L2)
	@sudo ip link set dev $(IF0) up
	@sudo ip link set dev $(IF1) up
	@sudo ip link set dev $(IF2) up
	@sudo ip link set dev $(IF3) up
	@sudo ip link set $(IF2) promisc on
	@sudo ip link set $(IF3) promisc on
	@ip link show $(IF0)
	@ip link show $(IF1)
	@ip link show $(IF2)
	@ip link show $(IF3)


# removing veth ethernet interfaces
teardown:
	@sudo ip link set dev $(IF0) down
	@sudo ip link set dev $(IF1) down
	@sudo ip link set dev $(IF2) down
	@sudo ip link set dev $(IF3) down
	@sudo ip link del $(IF0)
	@sudo ip link del $(IF1)
	@ip link show


clean:
	rm -rf $(OBJDIR) $(BINDIR)

