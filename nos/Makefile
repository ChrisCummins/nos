# The default goal is...
.DEFAULT_GOAL = all

# The binary file.
TARGET       := kernel

# Header file locations.
HEADER_FILES :=              	\
		assert.h	\
		fs.h		\
		gdt.h		\
		idt.h		\
		initrd.h	\
		isr.c		\
		kheap.h		\
		kstream.h	\
		multiboot.h	\
		ordered-array.h \
		paging.h	\
		panic.h		\
		port.h		\
		ports/pic.h	\
		ports/pit.h	\
		ports/tty.h	\
		string.h	\
		stdarg.h	\
		timer.h		\
		tty.h		\
		tty/ascii.h	\
		tty/colours.h 	\
		types.h		\
		util.h		\
		$(NULL)

# C source file locations.
SOURCES_C    :=			\
		fs.c		\
		gdt.c		\
		idt.c		\
		initrd.c	\
		isr.c		\
		kheap.c		\
		kstream.c	\
		main.c		\
		ordered-array.c \
		panic.c		\
		paging.c	\
		port.c		\
		string.c	\
		timer.c		\
		tty.c		\
		$(NULL)

# Assembly source file locations.
SOURCES_ASM  :=        		\
		boot.s		\
		gdt-flush.s	\
		idt-flush.s	\
		interrupt.s	\
		$(NULL)

# Generate our object files list.
OBJECT_FILES := $(patsubst %.s,%.o,$(SOURCES_ASM))
OBJECT_FILES += $(patsubst %.c,%.o,$(SOURCES_C))

# We need to set some compile and link configuration settings to build for x86
# architectures.
CFLAGS += \
	   -fno-builtin \
	   -fno-stack-protector \
	   -m32 \
	   -march=i386 \
	   -nostdinc \
	   -nostdlib \
	   $(NULL)

ASFLAGS += -felf

LDFLAGS += \
	   -melf_i386 \
           -Tlink.ld \
	   $(NULL)

# Build targets.
all: $(TARGET) install

$(TARGET): $(OBJECT_FILES) $(HEADER_FILES)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECT_FILES)

# Install targets
.PHONY: install

install:
	$(SUDO) $(CP) $(TARGET) ../floppy/kernel

# Compilation rules.
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

# Clean up binaries and object files.
mrproper clean:
	$(RM) $(OBJECT_FILES) $(TARGET)
