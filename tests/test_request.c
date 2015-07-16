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
	void *requester1;
	void *requester2;
	int i;

	context = zmq_ctx_new ();

	requester1 = zmq_socket (context, ZMQ_REQ);
	zmq_connect (requester1, "tcp://localhost:5555");

	requester2 = zmq_socket (context, ZMQ_REQ);
	zmq_connect (requester2, "tcp://localhost:5556");

	for (i = 0; i < 2; i++)
	{
		char *request;
		char *reply;

		request = "bing";
		printf ("Request: %s\n", request);
		zmq_send (requester1, request, strlen (request), 0);

		reply = s_recv (requester1);
		printf ("Reply: %s\n", reply);
		free (reply);

		request = "bong";
		printf ("Request: %s\n", request);
		zmq_send (requester2, request, strlen (request), 0);

		reply = s_recv (requester2);
		printf ("Reply: %s\n", reply);
		free (reply);
	}

	zmq_close (requester1);
	zmq_close (requester2);
	zmq_ctx_destroy (context);
	return 0;
}
