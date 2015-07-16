clear all;

zmq_request('init');

requester1 = zmq_request('add_requester', 'tcp://localhost:5555');
requester2 = zmq_request('add_requester', 'tcp://localhost:5556');

for i = 1:2
    zmq_request('send_request', requester1, 'bing');
    reply1 = zmq_request('receive_reply', requester1, 3000)

    zmq_request('send_request', requester2, 'bong');
    reply2 = zmq_request('receive_reply', requester2, 3000)
end

zmq_request('close');
