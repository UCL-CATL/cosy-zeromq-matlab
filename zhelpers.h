/*  =====================================================================
    zhelpers.h

    Helper header file for example applications.
    =====================================================================
*/

#ifndef __ZHELPERS_H_INCLUDED__
#define __ZHELPERS_H_INCLUDED__

#if (defined (WIN32))
#   include <windows.h>
#endif

#include <zmq.h>
#include <string.h>

/* Receive 0MQ string from socket and convert into C string
 * Caller must free returned string. Returns NULL if the context
 * is being terminated.
 */
static char *
s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    if (size > 255)
        size = 255;
    buffer [size] = 0;
    return strdup (buffer);
}

/* Convert C string to 0MQ string and send to socket */
static int
s_send (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}

#endif /* __ZHELPERS_H_INCLUDED__ */
