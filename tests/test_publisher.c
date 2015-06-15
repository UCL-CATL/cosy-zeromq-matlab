/* Publisher, sends messages similar to the Pupil. */

#include <stdio.h>
#include <stdlib.h>
#include "zhelpers.h"

#define MAX_MSG_LENGTH 255

int
main (int argc,
      char *argv[])
{
	char *endpoint;
	char *tag;
	void *context;
	void *publisher;
	char msg[MAX_MSG_LENGTH + 1] = ""; /* "+ 1" for the null character */
	int ok;

	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s <endpoint> <tag>\n", argv[0]);
		fprintf (stderr, "Example 1: %s tcp://*:5000 Pupil\n", argv[0]);
		fprintf (stderr, "Example 2: %s tcp://*:5001 Gaze\n", argv[0]);
		return EXIT_FAILURE;
	}

	endpoint = argv[1];
	tag = argv[2];

	context = zmq_ctx_new ();
	publisher = zmq_socket (context, ZMQ_PUB);
	ok = zmq_bind (publisher, endpoint);
	assert (ok == 0);

	/* Create message */
	assert (msg[0] == '\0');
	strncat (msg, tag, MAX_MSG_LENGTH);
	strncat (msg, "\nbar:gloups\n", MAX_MSG_LENGTH - strlen (tag));

	while (1)
	{
		s_send (publisher, msg);
		s_send (publisher, "Other\nblah:1337\n");
	}

	zmq_close (publisher);
	zmq_ctx_destroy (context);
	return EXIT_SUCCESS;
}
