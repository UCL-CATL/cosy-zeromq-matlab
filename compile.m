% Under GNU/Linux:
%mex zmq_subscriber.c -lczmq -lzmq

% Under Windows:
% It doesn't work well when the DLL is not in the same directory as the
% generated mex file. Maybe because there are spaces in the path to the
% installed DLL?
mex zmq_subscriber.c ...
    -I"C:\Program Files\ZeroMQ 4.0.4\include" ...
    -L"C:\Program Files\ZeroMQ 4.0.4\lib" ...
    -llibzmq-v100-mt-4_0_4
