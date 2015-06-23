/* Publisher, sends messages similar to the Pupil. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "zhelpers.h"

#define MAX_MSG_LENGTH 256
#define SLEEP_TIME 10 /* in ms */

int
main (int argc,
      char *argv[])
{
	char *endpoint;
	char *tag;
	void *context;
	void *publisher;
	int ok;
	int msg_num;

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

	msg_num = 1;
	while (1)
	{
		char msg[MAX_MSG_LENGTH];
		int i;

		usleep (SLEEP_TIME * 1000);

		snprintf (msg, MAX_MSG_LENGTH, "%s\nnum:%d\nfoo:", tag, msg_num);
		for (i = strlen (msg); i < MAX_MSG_LENGTH - 2; i++)
		{
			msg[i] = 'a';
		}
		msg[MAX_MSG_LENGTH - 2] = '\n';
		msg[MAX_MSG_LENGTH - 1] = '\0';
		assert (strlen (msg) == MAX_MSG_LENGTH - 1);

		s_send (publisher, msg);
		s_send (publisher, "Other\nblah:1337\n");

		msg_num++;
	}

	zmq_close (publisher);
	zmq_ctx_destroy (context);
	return EXIT_SUCCESS;
}
