/* ZeroMQ requester wrapper for Matlab */

#include <assert.h>
#include <zmq.h>
#include <mex.h>

/* Support multiple initializations, because if the Matlab script crashes, the process
 * isn't killed.
 */
static void *context = NULL;
static void *requester = NULL;

static void
close_zmq (void)
{
	if (requester != NULL)
	{
		zmq_close (requester);
		requester = NULL;
	}

	if (context != NULL)
	{
		zmq_ctx_destroy (context);
		context = NULL;
	}
}

static void
print_error (const char *msg)
{
	/* Clean-up before exit, so hopefully the Matlab script can be run again
	 * without restarting Matlab.
	 */
	close_zmq ();

	mexErrMsgTxt (msg);
}

static void
init_zmq (void)
{
	int ok;

	close_zmq ();

	assert (context == NULL);
	context = zmq_ctx_new ();

	assert (requester == NULL);
	requester = zmq_socket (context, ZMQ_REQ);
	ok = zmq_connect (requester, "tcp://localhost:5555");
	if (ok != 0)
	{
		print_error ("zmq_request error: impossible to connect to the end point.");
	}
}

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
		print_error ("zmq_request error: you must provide the command name and the arguments.");
	}

	command = mxArrayToString (args[0]);

	for (i = 0; command[i] != '\0'; i++)
	{
		command[i] = tolower (command[i]);
	}

	if (strcmp (command, "init") == 0)
	{
		if (n_return_values > 0)
		{
			print_error ("zmq_request error: init command: "
				     "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			print_error ("zmq_request error: init command: too many arguments.");
		}

		init_zmq ();
	}
	else if (strcmp (command, "close") == 0)
	{
		if (n_return_values > 0)
		{
			print_error ("zmq_request error: close command: "
				     "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			print_error ("zmq_request error: close command: too many arguments.");
		}

		close_zmq ();
	}

	mxFree (command);
}
