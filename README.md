cosy-zeromq-matlab
==================

Incomplete [ZeroMQ](http://zeromq.org/) bindings for Matlab.

Communication patterns supported:
- Request-Reply
- Publisher-Subscriber

For some functions, a timeout parameter can be given, to either block until a
message has arrived, or wait for a certain time, or do not block at all.

Developed at the Cognition and System division (COSY) of the [Institute of
NeuroSciences](http://www.uclouvain.be/en-ions.html) at the Université
Catholique de Louvain, Belgium.

We use the bindings to communicate with the [Pupil](https://pupil-labs.com/)
eye tracking software, in combination with the
[Psychtoolbox](http://psychtoolbox.org/).
