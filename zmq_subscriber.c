/* Subscriber - listen everything. */

#include <ctype.h>
#include <string.h>
/*#include "zhelpers.h"*/
#include "mex.h"

void
mexFunction (int n_return_values,
	     mxArray *return_values[],
	     int n_args,
	     const mxArray *args[])
{
	char *command;
	int i;

	if (n_args < 1)
	{
		mexErrMsgTxt ("zmq_subscriber error: you must provide the command name and the arguments.");
	}

	command = mxArrayToString (args[0]);

	for (i = 0; command[i] != '\0'; i++)
	{
		command[i] = tolower (command[i]);
	}

	if (strcmp (command, "init") == 0)
	{
		mexPrintf ("What did you say? init? really?\n");
	}
	else if (strcmp (command, "receive_next_message") == 0)
	{
		const char *field_names[] = {"message_type", "diameter"};
		mxArray *message_type;
		mxArray *diameter;

		if (n_return_values > 1)
		{
			mexErrMsgTxt ("zmq_subscriber error: receive_next_message command: "
				      "you cannot assign the result to more than one return variable.");
		}

		if (n_args > 1)
		{
			mexErrMsgTxt ("zmq_subscriber error: receive_next_message command: "
				      "too many arguments.");
		}

		return_values[0] = mxCreateStructMatrix (1, 1, 2, field_names);

		message_type = mxCreateString ("Pupil");
		mxSetFieldByNumber (return_values[0], 0, 0, message_type);

		diameter = mxCreateString ("6");
		mxSetFieldByNumber (return_values[0], 0, 1, diameter);
	}

	mxFree (command);
}

#if 0
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

	zmq_close (subscriber);
	zmq_ctx_destroy (context);
	return 0;
}
#endif
