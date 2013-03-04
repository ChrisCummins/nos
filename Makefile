# The default goal is...
.DEFAULT_GOAL = all

# Use V=1 to see full verbosity.
QUIET_  = @
QUIET   = $(QUIET_$(V))

ifeq ($(strip $(V)),)
MAKE_QUIET = -s
endif

export QUIET_ QUIET MAKE_QUIET

# Configuration variables (exported to sub-makes).
export AS      := nasm
export CC      := gcc
export CP      := cp
export LD      := ld
export LESS    := less
export LOSETUP := losetup
export MKDIR   := mkdir
export MOUNT   := mount
export MV      := mv
export RM      := rm -f
export RMDIR   := rmdir
export SHELL   := /bin/bash
export SUDO    := sudo
export UMOUNT  := mount

# The binary file.
KBUILD_TARGET  := floppy/kernel

# Include directories.
topdir         := $(PWD)

# Compiler flags.
CFLAGS         := \
                  -DDEBUG \
                  -pedantic \
                  -std=c99 \
                  -Wall \
                  -Wextra \
                  -Wstrict-prototypes \
                  $(NULL)

ASFLAGS        := $(NULL)

LDFLAGS        := $(NULL)

export ASFLAGS CFLAGS LDFLAGS

# We keep our headers in a seperate include directory.
KBUILD_CFLAGS    += -Iinclude

# We need to ensure that we don't link against any standard libraries.
KBUILD_CFLAGS    += -fno-builtin -fno-stack-protector -nostdinc -nostdlib

# We don't want any form of optimisations (yet).
KBUILD_CFLAGS    += -O0

# We are compiling for IA-32 x86.
KBUILD_CFLAGS    += -march=i386 -m32
KBUILD_ASFLAGS   += -felf
KBUILD_LDFLAGS   += -melf_i386

# Specify our link script.
KBUILD_LDFLAGS   += -Tlink.ld

export KBUILD_ASFLAGS KBUILD_CFLAGS KBUILD_LDFLAGS

# Header file locations.
KBUILD_H_FILES  =              		\
		  fs/fs.h		\
		  fs/initrd.h		\
		  kernel/assert.h	\
		  kernel/gdt.h		\
		  kernel/idt.h		\
		  kernel/isr.h		\
		  kernel/kstream.h	\
		  kernel/multiboot.h	\
		  kernel/panic.h	\
		  kernel/port.h		\
		  kernel/stdarg.h	\
		  kernel/timer.h	\
		  kernel/tty.h		\
		  kernel/types.h	\
		  kernel/util.h		\
		  lib/ordered-array.h	\
		  lib/string.h		\
		  mm/heap.h		\
		  mm/paging.h		\
		  ports/pic.h		\
		  ports/pit.h		\
		  ports/tty.h		\
		  sched/sched.h		\
		  sched/task.h		\
		  tty/ascii.h		\
		  tty/colours.h 	\
		  $(NULL)

# C source file locations.
KBUILD_SRC_C   :=			\
		  fs/fs.c		\
		  fs/initrd.c		\
		  kernel/gdt.c		\
		  kernel/idt.c		\
		  kernel/isr.c		\
		  kernel/kstream.c	\
		  kernel/main.c		\
		  kernel/panic.c	\
		  kernel/port.c		\
		  kernel/timer.c	\
		  kernel/tty.c		\
		  lib/ordered-array.c	\
		  lib/string.c		\
		  mm/heap.c		\
		  mm/paging.c		\
		  sched/sched.c		\
		  sched/task.c		\
		  $(NULL)

# Assembly source file locations.
KBUILD_SRC_ASM :=        		\
		  kernel/boot.s		\
		  kernel/gdt-flush.s	\
		  kernel/idt-flush.s	\
		  kernel/interrupt.s	\
		  sched/process.s	\
		  $(NULL)

# Generate our object and header files list.
KBUILD_OBJ_FILES := $(patsubst %.s,%.o,$(KBUILD_SRC_ASM))
KBUILD_OBJ_FILES += $(patsubst %.c,%.o,$(KBUILD_SRC_C))
KHBUILD_H_PATHS   = $(addprefix include/,$(KBUILD_H_FILES))

CURDIR := $(shell basename $$(pwd))

BUILD_DIRS := bochs docs tools

# Build targets.
.PHONY: all kernel $(BUILD_DIRS)

# Targets.
all: tools TAGS kernel floppy

kernel: $(KBUILD_TARGET)

$(KBUILD_TARGET): $(KBUILD_OBJ_FILES)
	@echo '  LD       '$@
	$(QUIET)$(LD) $(LDFLAGS) $(KBUILD_LDFLAGS) -o $(KBUILD_TARGET) $(KBUILD_OBJ_FILES)

$(BUILD_DIRS):
	@echo '  MAKE     '$@
	$(QUIET)$(MAKE) $(MAKE_QUIET) -C$@ all

# Compilation rules.
%.o: %.c $(KBUILD_H_PATHS)
	@echo '  CC       '$<
	$(QUIET)$(CC) $(CFLAGS) $(KBUILD_CFLAGS) -c -o $@ $<

%.o: %.s $(KBUILD_H_PATHS)
	@echo '  AS       '$<
	$(QUIET)$(AS) $(ASFLAGS) $(KBUILD_ASFLAGS) -o $@ $<

# Simulation targets.
.PHONY: log run floppy initrd

log:
	$(QUIET)less bochs/bochsout.txt

run:
	$(QUIET)$(SHELL) ./scripts/bochs.sh

floppy: initrd
	@echo '  GEN      floppy.img'
	$(QUIET)$(SHELL) ./scripts/mkfloppy.sh >/dev/null

initrd:
	@echo '  GEN      initrd.img'
	$(QUIET)$(SHELL) ./scripts/mkinitrd.sh >/dev/null

# Clean targets.
.PHONY: clean mrproper

clean:
	@echo '  CLEAN    kbuild'
	$(QUIET)$(RM) $(KBUILD_OBJ_FILES) $(KBUILD_TARGET)
	$(QUIET)for d in $(BUILD_DIRS); do 	    \
		echo '  CLEAN    '$$(basename $$d); \
		$(MAKE) $(MAKE_QUIET) -C $$d $@;    \
	done

mrproper:
	@echo '  CLEAN    kbuild'
	$(QUIET)$(RM) $(KBUILD_OBJ_FILES) $(KBUILD_TARGET)
	$(QUIET)for d in $(BUILD_DIRS); do 	    \
		echo '  CLEAN    '$$(basename $$d); \
		$(MAKE) $(MAKE_QUIET) -C $$d $@;    \
	done

# Miscellaneous targets.
.PHONY: TAGS mount umount help

TAGS:
	@echo '  TAGS     '
	$(QUIET)$(SHELL) ./scripts/tags.sh .

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
	@echo '* kernel     - Build the nos kernel'
	@echo '  tools      - Build all kernel tools'
	@echo ''
	@echo 'Image targets:'
	@echo '  floppy     - Generate bootable image from contents of floppy/'
	@echo '  initrd     - Generate an initrd image from contents of initrd/'
	@echo '  run        - Start a bochs session with the compiled kernel'
	@echo ''
	@echo 'Other targets:'
	@echo '  TAGS       - Generate a ./TAGS file in emacs format'
	@echo ''
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo ''
	@echo 'Execute "make" or "make all" to build all targets marked with [*]'
	@echo 'For further info see the ./README file'
