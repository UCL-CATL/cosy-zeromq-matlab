#include "multi_connector.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>
#include <mex.h>

#define MAX_SOCKETS 128

struct _MultiConnector
{
	void *context;
	void *sockets[MAX_SOCKETS];
	int next_socket_index;
};

MultiConnector *
multi_connector_new (void)
{
	MultiConnector *connector;

	connector = (MultiConnector *) malloc (sizeof (MultiConnector));
	memset (connector, 0, sizeof (MultiConnector));

	connector->context = zmq_ctx_new ();

	return connector;
}

void
multi_connector_free (MultiConnector *connector)
{
	int i;

	if (connector == NULL)
	{
		return;
	}

	for (i = 0; i < connector->next_socket_index; i++)
	{
		assert (connector->sockets[i] != NULL);
		zmq_close (connector->sockets[i]);
	}

	for (i = connector->next_socket_index; i < MAX_SOCKETS; i++)
	{
		assert (connector->sockets[i] == NULL);
	}

	if (connector->context != NULL)
	{
		zmq_ctx_destroy (connector->context);
	}

	free (connector);
}

/* Returns the socket_id */
int
multi_connector_add_socket (MultiConnector *connector,
			    int socket_type,
			    const char *end_point)
{
	void *new_socket;
	int index;
	int ok;

	if (connector->next_socket_index >= MAX_SOCKETS)
	{
		mexErrMsgTxt ("zmq multi_connector error: number of sockets limit reached, see the MAX_SOCKETS #define.");
	}

	new_socket = zmq_socket (connector->context, socket_type);

	ok = zmq_connect (new_socket, end_point);
	if (ok != 0)
	{
		mexErrMsgTxt ("zmq multi_connector error: impossible to connect to the end point.");
	}

	assert (new_socket != NULL);

	index = connector->next_socket_index;
	connector->sockets[index] = new_socket;
	connector->next_socket_index++;

	return index;
}

/* Returns whether the socket_id is valid (a boolean). */
int
multi_connector_valid_socket_id (MultiConnector *connector,
				 int socket_id)
{
	return (0 <= socket_id && socket_id < MAX_SOCKETS &&
		socket_id < connector->next_socket_index);
}

void *
multi_connector_get_socket (MultiConnector *connector,
			    int socket_id)
{
	if (!multi_connector_valid_socket_id (connector, socket_id))
	{
		return NULL;
	}

	return connector->sockets[socket_id];
}

static void
close_msg (zmq_msg_t *msg)
{
	int ok;

	ok = zmq_msg_close (msg);
	if (ok != 0)
	{
		mexErrMsgTxt ("zmq multi_connector error: impossible to close the message struct.");
	}
}

/* Receives the next zmq message as a string, with a timeout (in
 * milliseconds).
 * Free the return value with free() when no longer needed.
 */
char *
multi_connector_receive_next_message (MultiConnector *connector,
				      int socket_id,
				      double timeout)
{
	void *socket;
	zmq_msg_t msg;
	int ok;
	int time_elapsed;
	char *str = NULL;

	if (!multi_connector_valid_socket_id (connector, socket_id))
	{
		mexPrintf ("Invalid socket ID.\n");
		return NULL;
	}

	socket = connector->sockets[socket_id];
	assert (socket != NULL);

	ok = zmq_msg_init (&msg);
	if (ok != 0)
	{
		mexErrMsgTxt ("zmq multi_connector error: impossible to init the message struct.");
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
			utils_portable_sleep (1);
			time_elapsed++;
			continue;
		}

		break;
	}

	close_msg (&msg);

	return str;
}
