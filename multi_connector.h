#ifndef COSY_ZMQ_MULTI_CONNECTOR_H
#define COSY_ZMQ_MULTI_CONNECTOR_H

typedef struct _MultiConnector MultiConnector;

MultiConnector *	multi_connector_new			(void);

void			multi_connector_free			(MultiConnector *connector);

int			multi_connector_add_socket		(MultiConnector *connector,
								 int socket_type,
								 const char *end_point);

int			multi_connector_valid_socket_id		(MultiConnector *connector,
								 int socket_id);

void *			multi_connector_get_socket		(MultiConnector *connector,
								 int socket_id);

#endif /* COSY_ZMQ_MULTI_CONNECTOR_H */
