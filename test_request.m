clear all;

zmq_request('init');

requester = zmq_request('add_requester', 'tcp://localhost:5555');
zmq_request('send', requester, 'Hello');
reply = zmq_request('receive', requester, 3000)

zmq_request('close');
