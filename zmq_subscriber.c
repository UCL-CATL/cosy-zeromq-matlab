/* ZeroMQ subscriber wrapper for Matlab */

#include <zmq.h>
#include <string.h>
#include <assert.h>
#include "mex.h"

#define MAX_SUBSCRIBERS 128

/* Support multiple initializations, because if the Matlab script crashes, the process
 * isn't killed.
 */
static void *context = NULL;
static void *subscribers[MAX_SUBSCRIBERS] = { NULL };
static int next_subscriber_index = 0;

#if (defined (WIN32))
/* The function is available on GNU/Linux, but not on Windows.
 * Copy/paste from a version found on the web.
 */
static char *
strsep (char **sp, char *sep)
{
	char *p, *s;

	if (sp == NULL || *sp == NULL || **sp == '\0')
	{
		return NULL;
	}

	s = *sp;
	p = s + strcspn(s, sep);
	if (*p != '\0')
	{
		*p++ = '\0';
	}
	*sp = p;
	return s;
}
#endif

/* Receive 0MQ string from socket and convert into C string
 * Caller must free returned string. Returns NULL if the context
 * is being terminated.
 * Function taken from zhelpers.
 * TODO write a better function, instead of taking a fixed size.
 */
static char *
s_recv (void *socket)
{
	char buffer [256];
	int size = zmq_recv (socket, buffer, 255, 0);

	if (size == -1)
	{
		return NULL;
	}

	if (size > 255)
	{
		size = 255;
	}

	buffer [size] = '\0';
	return strdup (buffer);
}

/* Convert C string to 0MQ string and send to socket.
 * Function taken from zhelpers.
 */
static int
s_send (void *socket,
        char *string)
{
	int size = zmq_send (socket, string, strlen (string), 0);
	return size;
}

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
		mexErrMsgTxt ("zmq_subscriber error: number of subscribers limit reached, see the MAX_SUBSCRIBERS #define.");
	}

	new_subscriber = zmq_socket (context, ZMQ_SUB);

	ok = zmq_connect (new_subscriber, end_point);
	if (ok != 0)
	{
		mexErrMsgTxt ("zmq_subscriber error: impossible to connect to the end point.");
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
		mexErrMsgTxt ("zmq_subscriber error: impossible to set filter.");
	}
}

static void
str_array_free (char **array,
		int length)
{
	int i;

	if (array == NULL)
	{
		return;
	}

	for (i = 0; i < length; i++)
	{
		free (array[i]);
	}

	free (array);
}

static int
count_lines (char *str)
{
	int n_lines = 0;
	char *p;

	for (p = str; *p != '\0'; p++)
	{
		if (*p == '\n')
		{
			n_lines++;
		}
	}

	return n_lines;
}

/* Receives the next zmq message, parse it and return the field names and the
 * values. The return value is the number of fields (and thus the number of
 * values too).
 * Free 'field_names' and 'values' with str_array_free().
 */
static int
receive_message (int subscriber_id,
		 char ***field_names,
		 char ***values)
{
	void *subscriber;
	char *full_msg;
	int n_fields;
	char *msg;
	char *line;
	int field_num;

	*field_names = NULL;
	*values = NULL;

	if (!valid_subscriber_id (subscriber_id))
	{
		mexPrintf ("Invalid subscriber ID.\n");
		return 0;
	}

	subscriber = subscribers[subscriber_id];
	assert (subscriber != NULL);

	full_msg = s_recv (subscriber);
	n_fields = count_lines (full_msg);

	*field_names = (char **) malloc (sizeof (char *) * n_fields);
	*values = (char **) malloc (sizeof (char *) * n_fields);

	if (n_fields == 0)
	{
		return n_fields;
	}

	/* Get message type (first line). */
	msg = full_msg;
	line = strsep (&msg, "\n");

	field_num = 0;
	(*field_names)[field_num] = strdup ("message_type");
	(*values)[field_num] = strdup (line);

	/* Get next fields/values. */
	while (msg != NULL && msg[0] != '\0')
	{
		char *field_name;
		char *value;

		line = strsep (&msg, "\n");
		field_name = strsep (&line, ":");
		value = line;

		if (value == NULL)
		{
			value = "";
		}

		field_num++;
		assert (field_num < n_fields);

		(*field_names)[field_num] = strdup (field_name);
		(*values)[field_num] = strdup (value);
	}

	free (full_msg);

	return n_fields;
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
		mexErrMsgTxt ("zmq_subscriber error: you must provide the command name and the arguments.");
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
			mexErrMsgTxt ("zmq_subscriber error: init command: "
				      "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			mexErrMsgTxt ("zmq_subscriber error: init command: too many arguments.");
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
			mexErrMsgTxt ("zmq_subscriber error: add_subscriber command: "
				      "you cannot assign the result to more than one return variable.");
		}

		if (n_args > 2)
		{
			mexErrMsgTxt ("zmq_subscriber error: add_subscriber command: too many arguments.");
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
		int *arg_data;
		int subscriber_id;
		char *filter;

		if (n_return_values > 0)
		{
			mexErrMsgTxt ("zmq_subscriber error: add_filter command: "
				      "you cannot assign a result with this call.");
		}

		if (n_args > 3)
		{
			mexErrMsgTxt ("zmq_subscriber error: add_filter command: too many arguments.");
		}

		arg_data = (int *) mxGetData (args[1]);
		subscriber_id = *arg_data;

		filter = mxArrayToString (args[2]);

		add_filter (subscriber_id, filter);

		mxFree (filter);
	}
	else if (strcmp (command, "receive_next_message") == 0)
	{
		int *arg_data;
		int subscriber_id;
		char **field_names;
		char **values;
		int n_fields;
		int i;

		if (n_return_values > 1)
		{
			mexErrMsgTxt ("zmq_subscriber error: receive_next_message command: "
				      "you cannot assign the result to more than one return variable.");
		}

		if (n_args > 2)
		{
			mexErrMsgTxt ("zmq_subscriber error: receive_next_message command: "
				      "too many arguments.");
		}

		arg_data = (int *) mxGetData (args[1]);
		subscriber_id = *arg_data;

		n_fields = receive_message (subscriber_id, &field_names, &values);

		return_values[0] = mxCreateStructMatrix (1, 1, n_fields, (const char **)field_names);

		for (i = 0; i < n_fields; i++)
		{
			mxArray *value = mxCreateString (values[i]);
			mxSetFieldByNumber (return_values[0], 0, i, value);
		}

		str_array_free (field_names, n_fields);
		str_array_free (values, n_fields);
	}
	else if (strcmp (command, "close") == 0)
	{
		if (n_return_values > 0)
		{
			mexErrMsgTxt ("zmq_subscriber error: close command: "
				      "you cannot assign a result with this call.");
		}

		if (n_args > 1)
		{
			mexErrMsgTxt ("zmq_subscriber error: close command: too many arguments.");
		}

		close_zmq ();
	}

	mxFree (command);
}
