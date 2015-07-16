/* Request-reply test */

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "zhelpers.h"

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
		char *msg;

		msg = s_recv (responder);
		printf ("Received:%s\n", msg);
		free (msg);

		/* Do some "work" */
		sleep (1);

		zmq_send (responder, "world", 5, 0);
	}

	return 0;
}
