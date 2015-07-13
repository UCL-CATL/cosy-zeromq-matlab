#!/usr/bin/env bash

gcc -o test_publisher -O2 $(pkg-config --cflags --libs libczmq) test_publisher.c zhelpers.h
gcc -o test_subscriber -O2 $(pkg-config --cflags --libs libczmq) test_subscriber.c zhelpers.h
gcc -o test_request -O2 $(pkg-config --cflags --libs libczmq) test_request.c
gcc -o test_reply -O2 $(pkg-config --cflags --libs libczmq) test_reply.c
