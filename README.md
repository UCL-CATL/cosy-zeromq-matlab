cosy-zeromq-matlab
==================

Basic [ZeroMQ](http://zeromq.org/) bindings for Matlab.

Version: 1.0

Communication patterns supported:
- Request-Reply
- Publisher-Subscriber

Only the “client” part is supported in Matlab, i.e. it is possible to make
requests for the Request-Reply pattern, and subscribe to a publisher. But it is
neither possible to run a replier in Matlab nor a publisher.

For some functions, a timeout parameter can be given, to either block until a
message has arrived, or wait for a certain time, or do not block at all.

Works on both GNU/Linux and Windows.

cosy-zeromq-matlab is licensed under the GNU Lesser General Public License
version 3 or later.

Developed at the Cognition and System division (COSY) of the [Institute of
NeuroSciences](http://www.uclouvain.be/en-ions.html) at the Université
Catholique de Louvain, Belgium.

We use the bindings to communicate with the [Pupil](https://pupil-labs.com/)
eye tracking software, in combination with the
[Psychtoolbox](http://psychtoolbox.org/).

Install
-------

- You need to install ZeroMQ.
- In Matlab, go to the zmq/ directory and run `compile`. It should create the
  mex files. Note that there are also mex files available for Windows 32bit.
- Then see `help zmq_request` and `help zmq_subscriber`.

Tests
-----

In the `tests/` directory, there are some tests written in C that can be
compiled on GNU/Linux with the `make` command.

In the `zmq/` directory, there are also some *.m files to test the bindings.

For example run `tests/test_publisher` in a terminal and then execute
`zmq/test_subscriber.m` in Matlab.
