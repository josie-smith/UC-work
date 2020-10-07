# COSC364-assignment

This assignment consists of one server.py file that is intended to be runon multiple ports on the same machine. This simulates a network of routers, 
which can be configured using the conf files. The routers communicate using the RIPv2 protocol.


## Configuration File

### Grammar

```
<config>               ::= <config-line>|<config-line><config>
<config-line>          ::= <opt-ws><config-expression><opt-ws>"\n" |
                           <opt-ws><comment>"\n" |
                           <opt-ws><config-expression><opt-ws><comment>"\n"
<config-expression>    ::= <router-id-expression> |
                           <timer-expression> |
                           <neighbour-expression>

<router-id-expression> ::= "router-id"<ws><router-id>
<timer-expression>     ::= "timer"<ws><seconds>
<neighbour-expression> ::= "neighbour"<ws><router-id><ws><output-port><ws><input-port><ws><metric>

<router-id>            ::= [string parsable by python int()]
<input-port>           ::= [string parsable by python int()]
<output-port>          ::= [string parsable by python int()]
<metric>               ::= [string parsable by python int()]
<seconds>              ::= [string parsable by python int()]

<comment>              ::= "#"|"#"[any string with no line breaks]
<ws>                   ::= " "|" "<ws>|"\t"|"\t"<ws>
<opt-ws>               ::= ""|<ws>
```

Square bracket tokens are "intuitive" definitions, mainly just offloading the definition to the python language specification.

### Semantics and Value Constraints

`neighbour` expressions add to the set of neighbours, any other expressions override the value of their previous occurrences if they appear more than once. A valid configuration MUST have a `router-id` and at least one valid `neighbour` all other expressions are optional and have sensible default values.

`router-id` MUST be between 1 and 64000. `input-port` and `output-port` MUST all be between 1024 and 64000. `metric` must be between 1 and 16 and `timer` must be given the number of seconds between 1 and 1800.

Each port MUST only occur once in the `neighbour` expressions.

Each neighbour's `router-id` MUST be unique and distinct from the final `router-id` in the config file.

### Example config file

```
router-id 1
timer 5

# neighbour router-id output-port input-port metric
neighbour 2 3001 3000 1
neighbour 6 3010 3011 5
neighbour 7 3013 3012 8
```

## Logging, Output and Environment

The only output ever printed to `stdout` is the forwarding table. The way it's printed is that each line describes a forwarding destination and a blank line means the end of the table.

Any informational, debug, error, warning and critical log messages are printed to `stderr` in order to not interfere with the output of the program that might be used by other programs. As well as that, this allows the redirection of `stderr` to the system logs (much like a proper daemon designed for use in real systems).

The log verbosity can be set with the LOG_LEVEL environment variable and it defaults to 'INFO' where informational, warning, error and critical log messages are printed.

## Exiting

When the program receives a `SIGINT` or `SIGTERM` it will complete the current iteration of its main loop, close its sockets then exit gracefully with an exit code 0. If the program encounters a config error or can't find its assigned config file it will exit with an exit code 1. If the program fails to bind all input sockets it will exit with an exit code 2. The program will exit with an exit code 3 if it encounters any socket errors while using select(2) to wait on its input sockets.

## Testing

The `tester` module is useful for quickly building test networks, here is an example for creating a network of 3 routers which are fully connected.

```python
#!/usr/bin/env python

from tester import Tester

test = Tester('/path/to/server.py', timer=5)

# create routers
test.add_router(1)
test.add_router(2)
test.add_router(3)

# link them up, format: test.add_link(router_a, router_b, metric)
test.add_link(1, 2, 5)
test.add_link(1, 3, 2)
test.add_link(2, 3, 10)

test.generate_configs()
test.start()
```

The call `Tester.generate_configs` takes care of assigning unique port numbers to each simulated link. After which, `Tester.start` spins all the instances up inside tmux with a wrapper that holds the tmux window open after the server exits. So any error messages can be inspected.

The call `Tester.generate_iptables_scripts` generates a bunch of shell scripts for simulating link outages, and the links
coming back online.
