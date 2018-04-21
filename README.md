# raw_sockets

Virtual Ethernet interfaces and a not so transparent forwarding.


## Setup, Compile, Run, Teardown

The basic steps to setup and remove the application:

 1. `make setup`
 2. `cd test`
 3. `./open_wireshark_alice_and_bob.bash`
 4. Start capturing from interfaces `alice` and `bob`
    which sould be visible in the two wireshark
    instances.
 5. `cd .. && make`
 6. Start the built application `sudo ./bin/mitm`
 7. `cd test && ./test_ping_alice_to_bob.bash`
 8. `./test_ping_bob_to_alice.bash`
 9. Stop mitm application by pressing `Ctrl + C`
10. Quit the two wireshark instances
11. `make clean`
12. `make teardown`


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

  +----------------------------------+                                              +----------------------------------+
  |                                  |                                              |                                  |
  |  network namespace:              |                                              |  network namespace:              |
  |  princess-peach-castle           |                                              |  boss-blitz-galaxy               |
  |                                  |                                              |                                  |
  |                 +-----------+    |                                              |    +-----------+                 |
  |                 |           |    |            allegedly communication           |    |           |                 |
  |                 |  alice    | <----------------------------------------------------> |  bob      |                 |
  |                 |           |    |                                              |    |           |                 |
  |                 +------+----+    |                                              |    +----+------+                 |
  |                        ^         |                                              |         ^                        |
  +------------------------|---------+           <--+ real data path +--->          +---------|------------------------+
                           |                                                                  |
                           |                                                                  |
                           |            +-----------+                 +-----------+           |
                           |            |           |                 |           |           |
                           +----------->+  foo      |                 |  bar      +<----------+
                                        |           |                 |           |
                                        +--------+--+                 +--+--------+
                                                 ^                       ^
                                                 |     +-----------+     |
                                                 |     |           |     |
                                                 +---->+  mitm.c   +<----+
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

