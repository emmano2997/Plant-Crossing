CC      = gcc
CFLAGS  = -Wall -Iinclude
TARGET  = plant-crossing

SRC     = src/main.c src/camera.c src/arvore.c src/casa.c src/casa_interior.c src/mundo.c src/textura.c

ifeq ($(OS),Windows_NT)
    LIBS   = -lfreeglut -lopengl32 -lglu32 -lm
    TARGET := $(TARGET).exe
else
    LIBS   = -lGL -lGLU -lglut -lm
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f plant-crossing plant-crossing.exe