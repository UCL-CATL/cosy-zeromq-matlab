/* Publisher, sends messages similar to the Pupil. */

#include "zhelpers.h"

int
main (void)
{
	void *context;
	void *publisher;
	int ok;

	context = zmq_ctx_new ();
	publisher = zmq_socket (context, ZMQ_PUB);

	ok = zmq_bind (publisher, "tcp://*:5001");
	assert (ok == 0);

	while (1)
	{
		s_send (publisher, "Pupil\nfoo:6\nbar:gloups\n");
		s_send (publisher, "Gaze\nbar:10.64\n");
	}

	zmq_close (publisher);
	zmq_ctx_destroy (context);
	return 0;
}
