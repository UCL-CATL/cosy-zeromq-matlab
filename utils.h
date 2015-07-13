#ifndef COSY_ZMQ_UTILS_H
#define COSY_ZMQ_UTILS_H

#ifdef WIN32
char * strndup (const char *s, size_t n);
#endif

void portable_sleep (int milliseconds);

#endif /* COSY_ZMQ_UTILS_H */
