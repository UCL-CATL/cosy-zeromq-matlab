#!/usr/bin/env bash

gcc -o subscriber $(pkg-config --cflags --libs libzmq libczmq) subscriber.c zhelpers.h
