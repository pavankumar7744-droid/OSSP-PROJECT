CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lrt -pthread

SRCS = src/main.c src/producer.c src/consumer.c src/ipc.c src/signal_handler.c
OBJS = $(SRCS:.c=.o)
EXEC = producer_consumer

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC) input.bin output.txt
