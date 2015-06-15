#!/usr/bin/env bash

gcc -o test_publisher $(pkg-config --cflags --libs libczmq) test_publisher.c zhelpers.h
gcc -o test_subscriber $(pkg-config --cflags --libs libczmq) test_subscriber.c zhelpers.h
