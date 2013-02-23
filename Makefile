# The default goal is...
.DEFAULT_GOAL = all

# Use V=1 to see full verbosity.
QUIET_  = @
QUIET   = $(QUIET_$(V))
export QUIET_ QUIET

# Makefile variables.
SHELL  := /bin/bash
AS     := nasm
CC     := gcc
LD     := ld
RM     := rm -fv

export CC LD RM AS

# Include directories.
topdir  := $(PWD)

INCLUDE := \
           -I $(topdir) \
	   $(NULL)

# Compiler flags.
CFLAGS  := \
	   -fno-builtin \
	   -fno-stack-protector \
	   $(INCLUDE) \
	   -m32 \
	   -march=i386 \
	   -nostdinc \
	   -nostdlib \
	   -pedantic \
	   -std=c99 \
	   -Wall \
	   -Wextra \
	   -Wstrict-prototypes \
	   $(NULL)

ASFLAGS := \
	   -felf \
	   $(NULL)

LDFLAGS := \
	   -Tlink.ld \
	   -melf_i386 \
	   $(NULL)

export ASFLAGS CFLAGS LDFLAGS

SUBDIRS    := $(filter %/, $(wildcard ./*/))
SOURCEDIRS := nos

# Targets.
.PHONY: all kernel run log help clean TAGS

all: kernel

kernel:
	$(QUIET)$(MAKE) -Cnos all

run: kernel
	$(QUIET)$(SHELL) ./scripts/simulate.sh

log:
	$(QUIET)less bochsout.txt

clean:
	$(QUIET)for d in $(SUBDIRS); do \
		$(MAKE) -C $$d clean; \
	done

TAGS:
	$(QUIET)$(SHELL) ./scripts/tags.sh $(SOURCEDIRS)

help:
	@echo 'Cleaning targets:'
	@echo '  clean     - Remove generated files'
	@echo ''
	@echo 'Generic targets:'
	@echo '  all       - Build all targets marked with [*]'
	@echo '* kernel    - Build the base kernel'
	@echo ''
	@echo 'Other targets:'
	@echo '  run       - Run the kernel in an emulator'
	@echo '  TAGS      - Generate a ./TAGS file in emacs format'
	@echo ''
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo ''
	@echo 'Execute "make" or "make all" to build all targets marked with [*]'
	@echo 'For further info see the ./README file'
