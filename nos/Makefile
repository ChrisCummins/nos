# The default goal is...
.DEFAULT_GOAL = all

# The binary file.
TARGET       := kernel

# Header file locations.
HEADER_FILES :=              	\
		gdt.h		\
		idt.h		\
		isr.c		\
		kheap.h		\
		kstream.h	\
		memory.h	\
		paging.h	\
		port.h		\
		ports/pic.h	\
		ports/pit.h	\
		ports/tty.h	\
		string.h	\
		timer.h		\
		tty.h		\
		tty/ascii.h	\
		tty/colours.h 	\
		types.h		\
		util.h		\
		$(NULL)

# C source file locations.
SOURCES_C    :=			\
		gdt.c		\
		idt.c		\
		isr.c		\
		kheap.c		\
		kstream.c	\
		main.c		\
		memory.c	\
		paging.c	\
		port.c		\
		string.c	\
		timer.c		\
		tty.c		\
		util.c		\
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

CFLAGS += -DDEBUG

# Build targets.
all: $(TARGET)

$(TARGET): $(OBJECT_FILES) $(HEADER_FILES)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECT_FILES)

# Compilation rules.
.c.o:
	$(CC) $(CFLAGS) -c $<

.s.o:
	$(AS) $(ASFLAGS) $<

# Clean up binaries and object files.
clean:
	$(RM) $(OBJECT_FILES) $(TARGET)
