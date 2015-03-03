#!/usr/bin/env bash

gcc -o zmq_publisher $(pkg-config --cflags --libs libczmq) zmq_publisher.c zhelpers.h
