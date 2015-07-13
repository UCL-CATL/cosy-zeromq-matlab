/* ZeroMQ subscriber wrapper for Matlab */

#include <string.h>
#include <assert.h>
#include <mex.h>
#include <zmq.h>

#include "utils.h"

#define MAX_SUBSCRIBERS 128

/* Support multiple initializations, because if the Matlab script crashes, the process
 * isn't killed.
 */
static void *context = NULL;
static void *subscribers[MAX_SUBSCRIBERS] = { NULL };
static int next_subscriber_index = 0;

static void
close_zmq (void)
{
	int i;

	for (i = 0; i < next_subscriber_index; i++)
	{
		assert (subscribers[i] != NULL);
		zmq_close (subscribers[i]);
		subscribers[i] = NULL;
	}

	for (i = next_subscriber_index; i < MAX_SUBSCRIBERS; i++)
	{
		assert (subscribers[i] == NULL);
	}

	next_subscriber_index = 0;

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
close_msg (zmq_msg_t *msg)
{
	int ok;

	ok = zmq_msg_close (msg);
	if (ok != 0)
	{
		print_error ("zmq_subscriber error: impossible to close message struct.");
	}
}

static void
init_zmq (void)
{
	close_zmq ();

	assert (context == NULL);
	context = zmq_ctx_new ();

	assert (next_subscriber_index == 0);
}

static int
add_subscriber (const char *end_point)
{
	void *new_subscriber;
	int index;
	int ok;

	if (next_subscriber_index >= MAX_SUBSCRIBERS)
	{
		print_error ("zmq_subscriber error: number of subscribers limit reached, see the MAX_SUBSCRIBERS #define.");
	}

	new_subscriber = zmq_socket (context, ZMQ_SUB);

	ok = zmq_connect (new_subscriber, end_point);
	if (ok != 0)
	{
		print_error ("zmq_subscriber error: impossible to connect to the end point.");
	}

	assert (new_subscriber != NULL);

	index = next_subscriber_index;
	subscribers[index] = new_subscriber;
	next_subscriber_index++;

	return index;
}

static int
valid_subscriber_id (int subscriber_id)
{
	return (0 <= subscriber_id && subscriber_id < MAX_SUBSCRIBERS &&
		subscriber_id < next_subscriber_index);
}

static void
add_filter (int subscriber_id,
	    const char *filter)
{
	void *subscriber;
	int ok;

	if (!valid_subscriber_id (subscriber_id))
	{
		mexPrintf ("Invalid subscriber ID.\n");
		return;
	}

	subscriber = subscribers[subscriber_id];
	assert (subscriber != NULL);

	ok = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));
	if (ok != 0)
	{
		print_error ("zmq_subscriber error: impossible to set filter.");
	}
}

/* Receives the next zmq message as a string, with a timeout (in
 * milliseconds).
 * Free the return value with free() when no longer needed.
 */
static char *
receive_next_message (int subscriber_id,
		      double timeout)
{
	void *socket;
	zmq_msg_t msg;
	int ok;
	int time_elapsed;
	char *str = NULL;

	if (!valid_subscriber_id (subscriber_id))
	{
		mexPrintf ("Invalid subscriber ID.\n");
		return NULL;
	}

	socket = subscribers[subscriber_id];
	assert (socket != NULL);

	ok = zmq_msg_init (&msg);
	if (ok != 0)
	{
		print_error ("zmq_subscriber error: impossible to init message struct.");
	}

	time_elapsed = 0;
	while (1)
	{
		int n_bytes;

		n_bytes = zmq_msg_recv (&msg, socket, ZMQ_DONTWAIT);

		if (n_bytes > 0)
		{
			void *raw_data;

			raw_data = zmq_msg_data (&msg);
			str = strndup ((char *) raw_data, n_bytes);
		}
		else if (n_bytes == -1 &&
			 errno == EAGAIN &&
			 time_elapsed < timeout)
		{
			/* Sleep 1 ms and try again.
			 * Note: unfortunately, setting the ZMQ_RCVTIMEO option
			 * with zmq_setsockopt() makes Matlab to crash (with
			 * pthread in the backtrace). So do the timeout
			 * ourselves, by polling ZeroMQ every millisecond.
			 */
			portable_sleep (1);
			time_elapsed++;
			continue;
		}

		break;
	}

	close_msg (&msg);

	return str;
}

static int
get_subscriber_id (const mxArray *arg)
{
	int *arg_data;

	if (mxGetClassID (arg) != mxINT32_CLASS)
	{
		print_error ("zmq_subscriber error: the subscriber_id has an invalid type, it should be int32.");
	}

	arg_data = (int *) mxGetData (arg);
	return *arg_data;
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
		print_error ("zmq_subscriber error: you must provide the command name and the arguments.");
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
			print_error ("zmq_subscriber error: init command: "
				     "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			print_error ("zmq_subscriber error: init command: too many arguments.");
		}

		init_zmq ();
	}
	else if (strcmp (command, "add_subscriber") == 0)
	{
		char *end_point;
		int subscriber_id;
		int *ret_data;

		if (n_return_values > 1)
		{
			print_error ("zmq_subscriber error: add_subscriber command: "
				     "you cannot assign the result to more than one return variable.");
		}

		if (n_args > 2)
		{
			print_error ("zmq_subscriber error: add_subscriber command: too many arguments.");
		}

		end_point = mxArrayToString (args[1]);

		subscriber_id = add_subscriber (end_point);

		return_values[0] = mxCreateNumericMatrix (1, 1, mxINT32_CLASS, mxREAL);
		ret_data = (int *) mxGetData (return_values[0]);
		*ret_data = subscriber_id;

		mxFree (end_point);
	}
	else if (strcmp (command, "add_filter") == 0)
	{
		int subscriber_id;
		char *filter;

		if (n_return_values > 0)
		{
			print_error ("zmq_subscriber error: add_filter command: "
				     "you cannot assign a result with this call.");
		}

		if (n_args > 3)
		{
			print_error ("zmq_subscriber error: add_filter command: too many arguments.");
		}

		subscriber_id = get_subscriber_id (args[1]);

		if (mxGetClassID (args[2]) != mxCHAR_CLASS)
		{
			print_error ("zmq_subscriber error: add_filter command: "
				     "the filter argument has an invalid type, it should be a string.");
		}

		filter = mxArrayToString (args[2]);

		add_filter (subscriber_id, filter);

		mxFree (filter);
	}
	else if (strcmp (command, "receive_next_message") == 0)
	{
		double *arg_data;
		int subscriber_id;
		double timeout;
		char *msg;

		if (n_return_values > 1)
		{
			print_error ("zmq_subscriber error: receive_next_message command: "
				     "you cannot assign the result to more than one return variable.");
		}

		if (n_args > 3)
		{
			print_error ("zmq_subscriber error: receive_next_message command: "
				     "too many arguments.");
		}

		subscriber_id = get_subscriber_id (args[1]);

		/* It seems that numeric types from Matlab are encoded as
		 * doubles, even if there is no decimal separator (e.g. 3000).
		 */
		if (mxGetClassID (args[2]) != mxDOUBLE_CLASS)
		{
			print_error ("zmq_subscriber error: receive_next_message command: "
				     "the timeout has an invalid type, it should be a double.");
		}

		arg_data = (double *) mxGetData (args[2]);
		timeout = *arg_data;

		msg = receive_next_message (subscriber_id, timeout);

		if (msg != NULL)
		{
			return_values[0] = mxCreateString (msg);
		}
		else
		{
			return_values[0] = mxCreateDoubleScalar (mxGetNaN ());
		}

		free (msg);
	}
	else if (strcmp (command, "close") == 0)
	{
		if (n_return_values > 0)
		{
			print_error ("zmq_subscriber error: close command: "
				     "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			print_error ("zmq_subscriber error: close command: too many arguments.");
		}

		close_zmq ();
	}

	mxFree (command);
}
