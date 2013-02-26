# The default goal is...
.DEFAULT_GOAL = $(TARGET)

TARGET       := initrd-gen

SOURCES_C    := initrd-gen.c
OBJECT_FILES := $(patsubst %.c,%.o,$(SOURCES_C))

# Build targets.
all: $(TARGET)

$(TARGET): $(OBJECT_FILES)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECT_FILES)

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean mrproper

# Clean up binaries and object files.
clean:
	$(RM) $(OBJECT_FILES) $(TARGET)

mrproper: clean
