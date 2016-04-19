#ifndef COSY_ZMQ_UTILS_H
#define COSY_ZMQ_UTILS_H

#include <mex.h>

#ifdef _WIN32
char *		strndup				(const char *s, size_t n);
#endif

int		utils_get_socket_id		(const mxArray *arg);

#endif /* COSY_ZMQ_UTILS_H */
