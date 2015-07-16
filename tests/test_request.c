/* Request-reply test */

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "zhelpers.h"

int
main (void)
{
	void *context;
	void *requester;
	int i;

	printf ("Connecting to hello world server...\n");

	context = zmq_ctx_new ();
	requester = zmq_socket (context, ZMQ_REQ);
	zmq_connect (requester, "tcp://localhost:5555");

	for (i = 0; i < 10; i++)
	{
		char *msg;

		printf ("Sending hello %d...\n", i);
		zmq_send (requester, "hello", 5, 0);

		msg = s_recv (requester);
		printf ("Received %s\n", msg);
		free (msg);
	}

	zmq_close (requester);
	zmq_ctx_destroy (context);
	return 0;
}
