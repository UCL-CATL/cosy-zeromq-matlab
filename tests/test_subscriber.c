#include "zhelpers.h"

int
main (void)
{
	void *context;
	void *subscriber;
	char *filter;
	int i;
	int ok;

	context = zmq_ctx_new ();
	subscriber = zmq_socket (context, ZMQ_SUB);
	ok = zmq_connect (subscriber, "tcp://localhost:5001");
	assert (ok == 0);

	filter = "Pupil";
	ok = zmq_setsockopt (subscriber,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);

	filter = "Gaze";
	ok = zmq_setsockopt (subscriber,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);

	for (i = 0; i < 4; i++)
	{
		char *msg = s_recv (subscriber);
		printf ("%s\n", msg);
		free (msg);
	}

	zmq_close (subscriber);
	zmq_ctx_destroy (context);
	return 0;
}
