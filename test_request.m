clear all;

zmq_request('init');

requesters = [];
requesters(end + 1) = zmq_request('add_requester', 'tcp://localhost:5555');
requesters(end + 1) = zmq_request('add_requester', 'tcp://localhost:5556');

requesters = int32(requesters);

requests = {'bing', 'bong'};

for i = 1:2
    for requester_num = 1:length(requesters)
        requester = requesters(requester_num);
        request_msg = requests{requester_num};

        zmq_request('send_request', requester, request_msg);
        reply = zmq_request('receive_reply', requester, 3000)
        if isnan(reply)
            break;
        end
    end
end

zmq_request('close');
