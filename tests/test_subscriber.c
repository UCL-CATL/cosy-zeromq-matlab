#include "zhelpers.h"
#include <stdio.h>

int
main (void)
{
	void *context;
	void *subscriber1;
	void *subscriber2;
	char *filter;
	int i;
	int ok;

	context = zmq_ctx_new ();
	subscriber1 = zmq_socket (context, ZMQ_SUB);
	printf ("connecting...\n");
	ok = zmq_connect (subscriber1, "tcp://localhost:5000");
	assert (ok == 0);

	subscriber2 = zmq_socket (context, ZMQ_SUB);
	ok = zmq_connect (subscriber2, "tcp://localhost:5001");
	assert (ok == 0);
	printf ("connected.\n");

	printf ("set filters...\n");
	filter = "filter1";
	ok = zmq_setsockopt (subscriber1,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);

	filter = "filter2";
	ok = zmq_setsockopt (subscriber2,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);
	printf ("filters set.\n");

	printf ("receiving messages...\n");
	for (i = 0; i < 3; i++)
	{
		char *msg = s_recv (subscriber1);
		printf ("%s\n", msg);
		free (msg);
	}

	for (i = 0; i < 3; i++)
	{
		char *msg = s_recv (subscriber2);
		printf ("%s\n", msg);
		free (msg);
	}

	zmq_close (subscriber1);
	zmq_close (subscriber2);
	zmq_ctx_destroy (context);
	return 0;
}
