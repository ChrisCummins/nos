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
	   -DDEBUG \
	   $(INCLUDE) \
	   -pedantic \
	   -std=c99 \
	   -Wall \
	   -Wextra \
	   -Wstrict-prototypes \
	   $(NULL)

ASFLAGS := $(NULL)

LDFLAGS := $(NULL)

export ASFLAGS CFLAGS LDFLAGS

SUBDIRS    := $(filter %/, $(wildcard ./*/))
SOURCEDIRS := nos initrd

# Targets.
.PHONY: all run log help clean TAGS todo $(SOURCEDIRS)

all: $(SOURCEDIRS)

$(SOURCEDIRS):
	$(QUIET)$(MAKE) -C$@ all

run:
	$(QUIET)$(SHELL) ./scripts/simulate.sh

log:
	$(QUIET)less bochsout.txt
initrd.img:
	$(QUIET)$(SHELL) ./scripts/mkinitrd.sh

clean:
	$(QUIET)for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

TAGS:
	$(QUIET)$(SHELL) ./scripts/tags.sh $(SOURCEDIRS)

todo:
	$(QUIET)$(SHELL) ./scripts/todo.sh $(SOURCEDIRS)

help:
	@echo 'Cleaning targets:'
	@echo '  clean     - Remove generated files'
	@echo ''
	@echo 'Generic targets:'
	@echo '  all       - Build all targets marked with [*]'
	@echo '* nos       - Build the base kernel'
	@echo ''
	@echo 'Other targets:'
	@echo '  run       - Run the kernel in an emulator'
	@echo '  TAGS      - Generate a ./TAGS file in emacs format'
	@echo '  todo      - Show all TODO and FIXME tags in source files'
	@echo ''
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo ''
	@echo 'Execute "make" or "make all" to build all targets marked with [*]'
	@echo 'For further info see the ./README file'
