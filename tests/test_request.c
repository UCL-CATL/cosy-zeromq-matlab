/* Request-reply test */

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
		char buffer[10];
		printf ("Sending Hello %d...\n", i);
		zmq_send (requester, "Hello", 5, 0);
		zmq_recv (requester, buffer, 10, 0);
		printf ("Received World %d\n", i);
	}

	zmq_close (requester);
	zmq_ctx_destroy (context);
	return 0;
}
