/* Request-reply test */

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int
main (void)
{
	void *context;
	void *responder;
	int rc;

	context = zmq_ctx_new ();
	responder = zmq_socket (context, ZMQ_REP);
	rc = zmq_bind (responder, "tcp://*:5555");
	assert (rc == 0);

	while (1)
	{
		char buffer[10];

		zmq_recv (responder, buffer, 10, 0);
		printf ("Received Hello\n");

		/* Do some "work" */
		sleep (1);

		zmq_send (responder, "World", 5, 0);
	}

	return 0;
}
