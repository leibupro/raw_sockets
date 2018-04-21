#
#  ----------------------------------------------------------------------------
#  "THE BEER-WARE LICENSE" (Revision 42):
#  <pl@vqe.ch> wrote this file.  As long as you retain this notice you
#  can do whatever you want with this stuff. If we meet some day, and you think
#  this stuff is worth it, you can buy me a beer in return.   P. Leibundgut
#  ----------------------------------------------------------------------------
# 
#  File:      Makefile
#
#
#  Purpose:   - Setup a virtual interfaces to simulate
#               a forwarding between two network
#               interfaces.
#
#             - Compile and link the mitm application
#
#             - Clean up binaries
#
#             - Teardown virtual interfaces/
#               infrastructure.
#
#
#  Remarks:   - To setup the virtual interfaces
#               and build network namespaces
#               the ip command is required and
#               a kernel version >= 4.9 is
#               recommended.
#
#             - The virtual infrastructure is
#               well depicted in the README.md
#               file.
#
#
#  Date:      04/2018
#

SHELL := /bin/bash --login

IF0 := alice
IF1 := bob
IF2 := foo
IF3 := bar

IF0_L2 := 02:DE:AD:BE:EF:01
IF1_L2 := 02:DE:AD:BE:EF:02
IF2_L2 := 02:DE:AD:BE:EF:03
IF3_L2 := 02:DE:AD:BE:EF:04

IF0_L3 := 10.0.0.101/24
IF1_L3 := 10.0.0.102/24

NETNS0 := princess-peach-castle
NETNS1 := boss-blitz-galaxy

CC := gcc
LD := $(CC)

CF := -g -O0 -std=gnu89 -Wall -Wextra -Werror
LF := 

BINDIR := ./bin
OBJDIR := ./obj
SRCDIR := ./src
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
	@sudo ip netns add $(NETNS0)
	@sudo ip netns add $(NETNS1)
	@sudo ip link add $(IF0) type veth peer name $(IF2)
	@sudo ip link add $(IF1) type veth peer name $(IF3)
	@sudo ip link set dev $(IF0) address $(IF0_L2)
	@sudo ip link set dev $(IF1) address $(IF1_L2)
	@sudo ip link set dev $(IF2) address $(IF2_L2)
	@sudo ip link set dev $(IF3) address $(IF3_L2)
	@sudo sysctl -w net.ipv6.conf.$(IF2).disable_ipv6=1
	@sudo sysctl -w net.ipv6.conf.$(IF3).disable_ipv6=1
	@sudo ip link set $(IF2) promisc on
	@sudo ip link set $(IF3) promisc on
	@sudo ip link set dev $(IF2) arp off
	@sudo ip link set dev $(IF3) arp off
	@sudo ip link set $(IF2) multicast off
	@sudo ip link set $(IF3) multicast off
	@sudo ip link set $(IF0) netns $(NETNS0)
	@sudo ip link set $(IF1) netns $(NETNS1)
	@sudo ip netns exec $(NETNS0) ip addr add $(IF0_L3) dev $(IF0)
	@sudo ip netns exec $(NETNS1) ip addr add $(IF1_L3) dev $(IF1)
	@sudo ip netns exec $(NETNS0) ip link set dev $(IF0) up
	@sudo ip netns exec $(NETNS1) ip link set dev $(IF1) up
	@sudo ip link set dev $(IF2) up
	@sudo ip link set dev $(IF3) up
	@ip netns show
	@sudo ip netns exec $(NETNS0) ip addr show $(IF0)
	@sudo ip netns exec $(NETNS1) ip addr show $(IF1)
	@ip link show $(IF2)
	@ip link show $(IF3)


# removing veth ethernet interfaces
teardown:
	@sudo ip netns exec $(NETNS0) ip link set dev $(IF0) down
	@sudo ip netns exec $(NETNS1) ip link set dev $(IF1) down
	@sudo ip link set dev $(IF2) down
	@sudo ip link set dev $(IF3) down
	@sudo ip netns exec $(NETNS0) ip link del $(IF0)
	@sudo ip netns exec $(NETNS1) ip link del $(IF1)
	@sudo ip netns del $(NETNS0)
	@sudo ip netns del $(NETNS1)
	@ip link show


clean:
	rm -rf $(OBJDIR) $(BINDIR)

