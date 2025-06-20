CC = gcc
CFLAGS = `pkg-config --cflags ibus-1.0 glib-2.0`
LIBS = `pkg-config --libs ibus-1.0 glib-2.0`
SRCDIR = .
BUILDDIR = build
TARGET = ibus-telex-vn
SOURCES = main.c engine.c tieng.c am_giua.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean install debug

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /usr/lib/ibus/
	sudo chmod +x /usr/lib/ibus/$(TARGET)
	sudo cp ibus-telex-vn.xml /usr/share/ibus/component/

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

.SUFFIXES: .c .o