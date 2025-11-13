CC = gcc
CFLAGS = -Wall -I./include -g
SRCS = songList.c songControl.c album.c
OBJS = $(SRCS:.c=.o)
TARGET = a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
