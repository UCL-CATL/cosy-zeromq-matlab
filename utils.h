#ifndef COSY_ZMQ_UTILS_H
#define COSY_ZMQ_UTILS_H

#ifdef WIN32
char *		strndup				(const char *s, size_t n);
#endif

void		utils_portable_sleep		(int milliseconds);

int		utils_get_socket_id		(const mxArray *arg);

#endif /* COSY_ZMQ_UTILS_H */
