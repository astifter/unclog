## TL;DR

`unclog` is a zero-config, yet highly flexible logging library for C. It currently only supports POSIX-style OSes and needs pthread-style mutexes.

### Compile and Install

    ./autogen.sh && ./configure && make check && sudo make install

### Basics

Each logging message has a level and is created from a logging source. Only when the logging level of the message is greater or equal to the logging level of the source the message is actually processed.

A logging source gets a name when created, when a message from this source gets processed the message level, source name and actual logging message are handed to each logging sink.

A logging sink has a logging level as well, only when the message level is greater or equal to the sink level the message is processed by the sink. The sink then processes the message further according to the logging sinks porpose.

### Configuration

`unclog` is configured by either `./unclog.ini` in the current working directory of the executable or `/etc/unclog.ini` if there is no `unclog.ini` in the current directory. 

The configuration first sets default values and the used sinks and then configures the sinks and possibly the sources.

Here is an example `unclog.ini` which can be found in `src/examples`
```
[Defaults]
# default logging level for sources and sinks that do not have an explicitly set level
Level=Warning

# most logging sinks can print/store more or less details of a message,
# reconfigure defaults to not use File and Line information
Details=Time,Source,Message,Level
# the full details set would be Time,Source,File,Line,Message,Level

# define which sinks are configured for message processing
Sinks=libunclog_stderr.so

# now configure sink libunclog_stderr.so to have level Debug
[libunclog_stderr.so]
Level=Debug

# configure source main to have level Trace
[main]
Level=Trace
```
### Default Configuration

When no configuration files are found or found files do not specify the setting the following defaults are used:

```
[Defaults]
Level=Warning
Details=Time,Source,Message,Level
Sinks=libunclog_stderr.so
```

### Usage

Using `unclog` is simple, see the following `example1.c` from `src/examples`.

```
// only one include file is needed for basic usage
#include <unclog/unclog.h>

int main(int argc, char** argv) {
    // open a logging source "main", when this is called for the first time
    // during the process' lifetime the unclog.ini is searched for and
    // processed and unclog is initialized
    unclog_t* logger = unclog_open("main");
    // no create some logging messages on several logging levels for this
    // logging source
    UL_ER(logger, "error message: %d, logger is %p", 42, logger);
    UL_DE(logger, "debug message: %d, logger is %p", 43, logger);
    UL_TR(logger, "trace message: %d, logger is %p", 44, logger);
    // when the logger is not needed anymore it has to be closed. when this was
    // the last source to be closed unclog is de-initialized and all internal
    // memory is freed
    unclog_close(logger);
}
```

## The Name

I googled nclog (for new C logging library) and got results for unclog. Seemed like a good name, and since then *Bob's your unclog*.
