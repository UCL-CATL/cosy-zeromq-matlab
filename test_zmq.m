clear all;

%zmq_subscriber('init', 'tcp://localhost:5001', 'Pupil');
zmq_subscriber('init', 'tcp://192.168.1.1:5000', '', ...
    'C:\Users\sewilmet\Documents\MATLAB\zeromq\libzmq-v100-mt-4_0_4.dll');

for i = 1:5
    zmq_subscriber('receive_next_message')
end