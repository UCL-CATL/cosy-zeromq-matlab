/* Subscriber - listen everything. */

#include <zmq.h>
#include <windows.h>
#include <string.h>
#include <assert.h>
#include "mex.h"

/* Support multiple initializations, because if the Matlab program crashes, the process
 * isn't killed.
 */
static void *context = NULL;
static void *subscriber = NULL;

/* Boilerplate for loading DLL functions. */
typedef void * (* ZmqCtxNew) (void);
typedef int (* ZmqClose) (void *s);
typedef void * (* ZmqSocket) (void *, int type);
typedef int (* ZmqConnect) (void *s, const char *addr);
typedef int (* ZmqSetsockopt) (void *s, int option, const void *optval, size_t optvallen);
typedef int (* ZmqRecv) (void *s, void *buf, size_t len, int flags);
typedef int (* ZmqSend) (void *s, const void *buf, size_t len, int flags);

static ZmqCtxNew zmq_ctx_new_func = NULL;
static ZmqClose zmq_close_func = NULL;
static ZmqSocket zmq_socket_func = NULL;
static ZmqConnect zmq_connect_func = NULL;
static ZmqSetsockopt zmq_setsockopt_func = NULL;
static ZmqRecv zmq_recv_func = NULL;
static ZmqSend zmq_send_func = NULL;

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

static void
load_zmq_functions (const char *path_to_dll)
{
	static HINSTANCE handle = NULL;

	if (handle != NULL)
	{
		mexPrintf ("zmq_subscriber info: DLL file already loaded.\n");
		return;
	}

	handle = LoadLibrary (path_to_dll);

	if (handle == NULL)
	{
		mexErrMsgTxt ("zmq_subscriber error when loading the DLL.");
		return;
	}
	
	zmq_ctx_new_func = (ZmqCtxNew) GetProcAddress (handle, "zmq_ctx_new");
	zmq_close_func = (ZmqClose) GetProcAddress (handle, "zmq_close");
	zmq_socket_func = (ZmqSocket) GetProcAddress (handle, "zmq_socket");
	zmq_connect_func = (ZmqConnect) GetProcAddress (handle, "zmq_connect");
	zmq_setsockopt_func = (ZmqSetsockopt) GetProcAddress (handle, "zmq_setsockopt");
	zmq_recv_func = (ZmqRecv) GetProcAddress (handle, "zmq_recv");
	zmq_send_func = (ZmqSend) GetProcAddress (handle, "zmq_send");

	if (zmq_ctx_new_func == NULL ||
	    zmq_close_func == NULL ||
	    zmq_socket_func == NULL ||
	    zmq_connect_func == NULL ||
	    zmq_setsockopt_func == NULL ||
	    zmq_recv_func == NULL ||
	    zmq_send_func == NULL)
	{
		FreeLibrary (handle);
		handle = NULL;
		mexErrMsgTxt ("zmq_subscriber error: some function(s) in the DLL file can not be loaded.");
		return;
	}

	mexPrintf ("zmq_subscriber info: DLL file initialization OK.\n");
}

/* Receive 0MQ string from socket and convert into C string
 * Caller must free returned string. Returns NULL if the context
 * is being terminated.
 * Function taken from zhelpers.
 */
static char *
s_recv (void *socket) {
	char buffer [256];
	int size = zmq_recv_func (socket, buffer, 255, 0);
	
	if (size == -1)
	{
		return NULL;
	}
	
	if (size > 255)
	{
		size = 255;
	}
	
	buffer [size] = 0;
	return strdup (buffer);
}

/* Convert C string to 0MQ string and send to socket.
 * Function taken from zhelpers.
 */
static int
s_send (void *socket, char *string) {
	int size = zmq_send_func (socket, string, strlen (string), 0);
	return size;
}

static void
init_zmq (const char *end_point,
	  const char *filter)
{
	int ok;

	if (context == NULL)
	{
		context = zmq_ctx_new_func ();
	}

	if (subscriber != NULL)
	{
		zmq_close_func (subscriber);
	}

	subscriber = zmq_socket_func (context, ZMQ_SUB);

	ok = zmq_connect_func (subscriber, end_point);
	if (ok != 0)
	{
		mexErrMsgTxt ("zmq_subscriber error: impossible to connect to the end point.");
	}

	ok = zmq_setsockopt_func (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));
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
receive_message (char ***field_names,
		 char ***values)
{
	char *full_msg;
	int n_fields;
	char *msg;
	char *line;
	int field_num;

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
		char *end_point;
		char *filter;
		char *path_to_dll;

		if (n_return_values > 0)
		{
			mexErrMsgTxt ("zmq_subscriber error: init command: "
				      "you cannot assign a result with this call.");
		}

		if (n_args > 4)
		{
			mexErrMsgTxt ("zmq_subscriber error: init command: too many arguments.");
		}

		end_point = mxArrayToString (args[1]);
		filter = mxArrayToString (args[2]);
		path_to_dll = mxArrayToString (args[3]);

		load_zmq_functions (path_to_dll);
		init_zmq (end_point, filter);

		mxFree (end_point);
		mxFree (filter);
		mxFree (path_to_dll);
	}
	else if (strcmp (command, "receive_next_message") == 0)
	{
		char **field_names;
		char **values;
		int n_fields;
		int i;

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

		n_fields = receive_message (&field_names, &values);

		return_values[0] = mxCreateStructMatrix (1, 1, n_fields, (const char **)field_names);

		for (i = 0; i < n_fields; i++)
		{
			mxArray *value = mxCreateString (values[i]);
			mxSetFieldByNumber (return_values[0], 0, i, value);
		}

		str_array_free (field_names, n_fields);
		str_array_free (values, n_fields);
	}

	mxFree (command);
}
