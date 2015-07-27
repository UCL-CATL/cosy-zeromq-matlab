clear all;

zmq_subscriber('init');

subscriber_eye = zmq_subscriber('add_subscriber', 'tcp://localhost:5000');
zmq_subscriber('add_filter', subscriber_eye, 'Pupil');

subscriber_world = zmq_subscriber('add_subscriber', 'tcp://localhost:5001');
zmq_subscriber('add_filter', subscriber_world, 'Gaze');

% Receive messages at 100 Hz during 2 minutes (the publisher must send messages
% at 100 Hz).
%last = 100 * 60 * 2;
%for i = 1:last
%    msg = zmq_subscriber('receive_next_message', subscriber_eye, 3000);
%    if i == 1 || i == last
%        msg
%    end
%end

%for i = 1:last
%    msg = zmq_subscriber('receive_next_message', subscriber_world, 3000);
%    if i == 1 || i == last
%        msg
%    end
%end

msg_eye = zmq_subscriber('receive_next_message', subscriber_eye, 0)
msg_eye = zmq_subscriber('receive_next_message', subscriber_eye, 0)
msg_eye = zmq_subscriber('receive_next_message', subscriber_eye, 3000)

msg_world = zmq_subscriber('receive_next_message', subscriber_world, 3000)
msg_world = zmq_subscriber('receive_next_message', subscriber_world, 2000)
msg_world = zmq_subscriber('receive_next_message', subscriber_world, 1000)

msg_eye = zmq_subscriber('receive_next_message', subscriber_eye, -1)

zmq_subscriber('close');
