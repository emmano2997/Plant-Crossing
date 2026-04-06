CC      = gcc
CFLAGS  = -Wall -Iinclude
LIBS    = -lGL -lGLU -lglut -lm
TARGET  = plant-crossing
SRC     = src/main.c src/camera.c src/arvore.c src/casa.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)