#include "zhelpers.h"

int
main (void)
{
	void *context;
	void *subscriber_eye;
	void *subscriber_world;
	char *filter;
	int i;
	int ok;

	context = zmq_ctx_new ();
	subscriber_eye = zmq_socket (context, ZMQ_SUB);
	ok = zmq_connect (subscriber_eye, "tcp://localhost:5000");
	assert (ok == 0);

	subscriber_world = zmq_socket (context, ZMQ_SUB);
	ok = zmq_connect (subscriber_world, "tcp://localhost:5001");
	assert (ok == 0);

	filter = "Pupil";
	ok = zmq_setsockopt (subscriber_eye,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);

	filter = "Gaze";
	ok = zmq_setsockopt (subscriber_world,
			     ZMQ_SUBSCRIBE,
			     filter,
			     strlen (filter));
	assert (ok == 0);

	for (i = 0; i < 4; i++)
	{
		char *msg = s_recv (subscriber_eye);
		printf ("Eye:\n%s\n", msg);
		free (msg);
	}

	for (i = 0; i < 4; i++)
	{
		char *msg = s_recv (subscriber_world);
		printf ("World:\n%s\n", msg);
		free (msg);
	}

	zmq_close (subscriber_eye);
	zmq_close (subscriber_world);
	zmq_ctx_destroy (context);
	return 0;
}
