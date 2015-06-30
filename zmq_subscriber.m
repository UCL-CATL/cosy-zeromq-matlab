% zmq_subscriber  ZeroMQ subscriber.
%    zmq_subscriber('INIT')
%    Initializes ZeroMQ.
%
%    subscriber_id = zmq_subscriber('ADD_SUBSCRIBER', end_point)
%    Adds a subscriber (see example below).
%
%    zmq_subscriber('ADD_FILTER', subscriber_id, filter)
%    Adds a filter to an existing subscriber. To receive messages, adding a
%    filter is mandatory. If you want to subscribe to all messages, the filter
%    parameter must be an empty string. (it's exactly how you would do it in C
%    with zmq_setsockopt()).
%
%    message = zmq_subscriber('RECEIVE_NEXT_MESSAGE', subscriber_id, timeout)
%    Receives the next message of a specific subscriber. If 'timeout' is 0,
%    this function doesn't block and returns NaN if there is no message
%    on the queue. If 'timeout' is greater than 0, the program exits with an
%    error if there is no messages after 'timeout' milliseconds, probably
%    because the publisher is not connected.
%    The return value 'message' is either a NaN or a struct. This function
%    assumes that the message received with ZeroMQ is:
%        - a string
%        - lines are terminated by LF (\n) characters
%        - the first line contains a tag (the message_type in the struct)
%        - the following lines have the format field:value.
%        - the last line must also be terminated by a LF character (otherwise
%          the field:value isn't extracted).
%
%    zmq_subscriber('CLOSE')
%    Closes all subscribers, to free resources. It is important to call this
%    function when the subscribers are no longer needed. Otherwise problems can
%    occur, especially after running several times the same script.
%
%    Example:
%    zmq_subscriber('init');
%
%    subscriber_eye = zmq_subscriber('add_subscriber', 'tcp://localhost:5000');
%    zmq_subscriber('add_filter', subscriber_eye, 'Pupil');
%
%    msg = zmq_subscriber('receive_next_message', subscriber_eye, 3000)
%
%    zmq_subscriber('close');
%
% Sébastien Wilmet, 2015.
