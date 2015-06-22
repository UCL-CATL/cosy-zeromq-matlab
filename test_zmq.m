clear all;

zmq_subscriber('init');

subscriber_eye = zmq_subscriber('add_subscriber', 'tcp://localhost:5000');
zmq_subscriber('add_filter', subscriber_eye, 'Pupil');

subscriber_world = zmq_subscriber('add_subscriber', 'tcp://localhost:5001');
zmq_subscriber('add_filter', subscriber_world, 'Gaze');

for i = 1:3
    zmq_subscriber('receive_next_message', subscriber_eye)
end

for i = 1:3
    zmq_subscriber('receive_next_message', subscriber_world)
end

zmq_subscriber('close');
