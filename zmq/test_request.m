clear all;

zmq_request('init');

requesters = [];
requesters(end + 1) = zmq_request('add_requester', 'tcp://localhost:5555');
requesters(end + 1) = zmq_request('add_requester', 'tcp://localhost:5556');
disp('Requesters connected');

requesters = int32(requesters);

requests = {'bing', 'bong'};

aborted = false;
for i = 1:2
    if aborted
        break;
    end

    for requester_num = 1:length(requesters)
        requester = requesters(requester_num);
        request_msg = requests{requester_num};

        disp('Send request...');
        zmq_request('send_request', requester, request_msg);
        disp('...done.');

        disp('Receive reply...');
        reply = zmq_request('receive_reply', requester, 3000)
        if isnan(reply)
            aborted = true;
            break;
        end
    end
end

zmq_request('close');
disp('Closed');
