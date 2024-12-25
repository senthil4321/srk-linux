# SRK Linux Project
This project demonstrates various capabilities in Linux using C programs.

## Programs

1. **Signal Handler** (`srk-signal-handler/signal_handler.c`): Demonstrates handling various signals.
2. **Capabilities** (`srk-capabilities/srk-capabilities.c`): Demonstrates checking and printing process capabilities.
3. **Syslog** (`srk-syslog/srk-syslog.c`): Demonstrates logging messages to the syslog.

## Compilation

To compile all programs, run:

```sh
make
```

## Running the Programs

### Signal Handler

To run the signal handler program:

```sh
./srk-signal-handler/signal_handler
```

### Capabilities

To run the capabilities program:

```sh
./srk-capabilities/srk-capabilities
```

### Syslog

To run the syslog program:

```sh
./srk-syslog/srk-syslog
```

## Cleaning Up

To clean up the compiled binaries, run:

```sh
make clean
```

## How to compile the porgram?

``` bash
sudo apt-get install libcap-dev

```

```bash
gcc srk-capabilites.c -lcap -o srk-capabilites

```
