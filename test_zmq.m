clear all;

%zmq_subscriber('init', 'tcp://localhost:5001', 'Pupil');
zmq_subscriber('init', 'tcp://192.168.1.1:5000', '');

for i = 1:5
    zmq_subscriber('receive_next_message')
end
