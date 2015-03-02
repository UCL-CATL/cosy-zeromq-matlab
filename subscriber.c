/* Subscriber - listen everything. */

#include "zhelpers.h"

int
main (int argc,
      char *argv[])
{
	void *context;
	void *subscriber;
	int ok;
	char *filter;

	context = zmq_ctx_new ();
	subscriber = zmq_socket (context, ZMQ_SUB);
	ok = zmq_connect (subscriber, "tcp://192.168.1.1:5000");
	assert (ok == 0);

	filter = "Pupil";
	printf ("Subscribe to: %s\n", filter);

	ok = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));
	assert (ok == 0);

	while (1)
	{
		char *str = s_recv (subscriber);
		printf ("%s\n", str);
		free (str);
	}

//	zmq_close (subscriber);
//	zmq_ctx_destroy (context);
	return 0;
}
