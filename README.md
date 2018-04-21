# raw_sockets

Virtual Ethernet interfaces and a not so transparent forwarding.


## Setup, Compile, Run, Teardown

The basic steps to setup and remove the application:

1. `make setup`
2. `make`
3. `sudo ./bin/mitm`
4. `sudo python2 ./src/py/rtc_frame_sender.py`
5. `make clean`
6. `make teardown`


## Scenario

The makefile targets `setup` and `teardown` do setup four
virtual ethernet interfaces:

1. alice
2. bob
3. foo
4. bar

The interfaces are connected as follows:

```
alice <--> foo
bob   <--> bar
```

... where `foo` and `bar` are operating in promiscuous mode.

The `mitm.c` application opens two raw sockets on the interfaces 
`foo` and `bar` and acts as a so called man in the middle. 

`mitm.c` is obviously able to modify or "poison" the frame 
content which is designated to be transferred between 
`alice` and `bob`.


### Scenario (graphical)

```


     +-----------+                             +-----------+
     |           |   allegedly communication   |           |
     |  alice    |<--------------------------->|  bob      |
     |           |                             |           |
     +-----------+                             +-----------+
              ^                                   ^
              |     <--- real data path ---->     |
              |                                   |
              v                                   v
           +-----------+                 +-----------+
           |           |                 |           |
           |  foo      |                 |  bar      |
           |           |                 |           |
           +-----------+                 +-----------+
                    ^                       ^
                    |     +-----------+     |
                    |     |           |     |
                    +---->|  mitm.c   |<----+
                          |           |
                          +-----------+


```


## Further work

This exemplary application sets up and uses virtual ethernet
network interfaces for demonstration purposes and can be
run and tested even on a machine which has no real interfaces.

But the application can easily be adjusted to work with real
hardware network interfaces, in fact, only the interface name
definitions have to be altered.

