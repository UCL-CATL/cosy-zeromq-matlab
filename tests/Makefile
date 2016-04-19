CC = gcc
CFLAGS = -W `pkg-config --cflags libczmq`
LDFLAGS = `pkg-config --libs libczmq`
EXECUTABLES = test_publisher test_subscriber test_request test_reply

.PHONY: clean

test_publisher: test_publisher.c zhelpers.h
test_subscriber: test_subscriber.c zhelpers.h

test_request: test_request.c zhelpers.h
test_reply: test_reply.c zhelpers.h

all: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES)
