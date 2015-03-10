% Under GNU/Linux:
%mex zmq_subscriber.c -lczmq -lzmq

% Under Windows:
mex zmq_subscriber.c libzmq-v100-mt-4_0_4.lib ...
    -I"C:\Program Files\ZeroMQ 4.0.4\include"