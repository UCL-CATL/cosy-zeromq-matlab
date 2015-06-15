#!/usr/bin/env bash

gcc -o zmq_publisher $(pkg-config --cflags --libs libczmq) zmq_publisher.c zhelpers.h
gcc -o test_subscriber $(pkg-config --cflags --libs libczmq) test_subscriber.c zhelpers.h
