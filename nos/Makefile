# The default goal is...
.DEFAULT_GOAL = all

# The binary file.
TARGET       := kernel

# Header file locations.
HEADER_FILES  =              		\
		assert.h		\
		fs.h			\
		gdt.h			\
		idt.h			\
		initrd.h		\
		isr.h			\
		kheap.h			\
		kstream.h		\
		mm/paging.h		\
		multiboot.h		\
		ordered-array.h		\
		panic.h			\
		port.h			\
		ports/pic.h		\
		ports/pit.h		\
		ports/tty.h		\
		sched/sched.h		\
		sched/task.h		\
		stdarg.h		\
		string.h		\
		timer.h			\
		tty.h			\
		tty/ascii.h		\
		tty/colours.h 		\
		types.h			\
		util.h			\
		$(NULL)

# C source file locations.
SOURCES_C    :=				\
		fs.c			\
		gdt.c			\
		idt.c			\
		initrd.c		\
		isr.c			\
		kheap.c			\
		kstream.c		\
		main.c			\
		mm/paging.c		\
		ordered-array.c 	\
		panic.c			\
		port.c			\
		sched/sched.c		\
		sched/task.c		\
		string.c		\
		timer.c			\
		tty.c			\
		$(NULL)

# Assembly source file locations.
SOURCES_ASM  :=        			\
		boot.s			\
		gdt-flush.s		\
		idt-flush.s		\
		interrupt.s		\
		sched/process.s		\
		$(NULL)

# Generate our object and header files list.
OBJECT_FILES     := $(patsubst %.s,%.o,$(SOURCES_ASM))
OBJECT_FILES     += $(patsubst %.c,%.o,$(SOURCES_C))
HEADER_FILE_PATHS = $(addprefix include/, $(HEADER_FILES))

# We keep our headers in a seperate include directory.
CFLAGS  += -Iinclude

# We need to ensure that we don't link against any standard libraries.
CFLAGS  += -fno-builtin -fno-stack-protector -nostdinc -nostdlib

# We don't want any form of optimisations (yet).
CFLAGS  += -O0

# We are compiling for IA-32 x86.
CFLAGS  += -march=i386 -m32
ASFLAGS += -felf
LDFLAGS += -melf_i386

# Specify our link script.
LDFLAGS += -Tlink.ld

# Build targets.
all: $(TARGET) install

$(TARGET): $(OBJECT_FILES)
	@echo '  LD       '$@
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECT_FILES)

# Install targets.
.PHONY: install

install:
	$(SUDO) $(CP) $(TARGET) ../floppy/kernel

CURDIR := $(shell basename $$(pwd))

# Compilation rules.
%.o: %.c $(HEADER_FILE_PATHS)
	@echo '  CC       '$<
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s $(HEADER_FILE_PATHS)
	@echo '  AS       '$<
	$(AS) $(ASFLAGS) -o $@ $<

# Clean up binaries and object files.
mrproper clean:
	$(RM) $(OBJECT_FILES) $(TARGET)
