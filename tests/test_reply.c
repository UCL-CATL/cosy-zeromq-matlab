/* Request-reply test */

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "zhelpers.h"

int
main (int argc,
      char *argv[])
{
	char *endpoint;
	char *reply;
	void *context;
	void *responder;
	int rc;

	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s <endpoint> <reply>\n", argv[0]);
		fprintf (stderr, "Example 1: %s tcp://*:5555 foo\n", argv[0]);
		fprintf (stderr, "Example 2: %s tcp://*:5556 bar\n", argv[0]);
		return EXIT_FAILURE;
	}

	endpoint = argv[1];
	reply = argv[2];

	context = zmq_ctx_new ();
	responder = zmq_socket (context, ZMQ_REP);
	rc = zmq_bind (responder, endpoint);
	assert (rc == 0);

	while (1)
	{
		char *request;

		request = s_recv (responder);
		printf ("Request: %s\n", request);
		free (request);

		/* Do some "work" */
		sleep (1);

		zmq_send (responder, reply, strlen (reply), 0);
	}

	return EXIT_SUCCESS;
}
