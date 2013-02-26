# The default goal is...
.DEFAULT_GOAL = all

# Use V=1 to see full verbosity.
QUIET_  = @
QUIET   = $(QUIET_$(V))
export QUIET_ QUIET

# Configuration variables (exported to sub-makes).
export AS      := nasm
export CC      := gcc
export CP      := cp
export LD      := ld
export LESS    := less
export LOSETUP := losetup
export MKDIR   := mkdir
export MOUNT   := mount
export RM      := rm -fv
export RMDIR   := rmdir
export SHELL   := /bin/bash
export SUDO    := sudo
export UMOUNT  := mount

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
SOURCEDIRS := nos initrd-gen

# Build targets.
.PHONY: all $(SOURCEDIRS) help

all: $(SOURCEDIRS) floppy

$(SOURCEDIRS):
	$(QUIET)$(MAKE) -C$@ all

# Simulation targets.
.PHONY: log run floppy initrd

log:
	less bochs/bochsout.txt

run:
	$(QUIET)$(SHELL) ./scripts/bochs.sh

# Temporary mountpoint for constructing boot image.
MOUNTPOINT = loop0

floppy: initrd
	$(QUIET)$(SHELL) ./scripts/mkfloppy.sh

initrd:
	$(QUIET)$(SHELL) ./scripts/mkinitrd.sh

# Clean targets.
.PHONY: clean mrproper

clean:
	$(QUIET)for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

mrproper:
	$(QUIET)for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

# Miscellaneous targets.
.PHONY: TAGS mount umount help

TAGS:
	$(QUIET)$(SHELL) ./scripts/tags.sh $(SOURCEDIRS)

mount:
	$(QUIET)$(SHELL) ./scripts/mkfloppy.sh mount
	@echo 'floppy.img mounted at loop0/'

umount:
	$(QUIET)$(SHELL) ./scripts/mkfloppy.sh umount

help:
	@echo 'Cleaning targets:'
	@echo '  clean      - Remove generated files'
	@echo '  mrproper   - Remove all generated files + logs'
	@echo ''
	@echo 'Generic targets:'
	@echo '  all        - Build all targets marked with [*]'
	@echo '* initrd-gen - Build the initrd-gen program'
	@echo '* nos        - Build the base kernel'
	@echo ''
	@echo 'Image targets:'
	@echo '* floppy     - Generate bootable image from contents of floppy/'
	@echo '* initrd     - Generate an initrd image from contents of initrd/'
	@echo '  run        - Start a bochs session with the compiled kernel'
	@echo ''
	@echo 'Other targets:'
	@echo '  TAGS       - Generate a ./TAGS file in emacs format'
	@echo ''
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo ''
	@echo 'Execute "make" or "make all" to build all targets marked with [*]'
	@echo 'For further info see the ./README file'
