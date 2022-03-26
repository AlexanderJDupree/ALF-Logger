# ALF Top Level Design

## Requirements

- High Performance

  - ALF should have minimal impact on runtime execution. It should be an ideal tool for debugging timing issues, context switches, interrupts. etc. It should also make it ideal for field debugging and clients should feel comfortable leaving ALF enabled in release builds.

- Compact

  - ALF's memory footprint will be minimal and make it ideal for embedded devices with memory constraints.

- Ergonomic

  - ALF should be as ergonomic and natural to use as a `printf`. It would be ideal if clients can just search and replace their current printf statements with ALF macros.

- Configurable
  - There's always Another Logging Framework because the current logging framework doesn't have timestamps, or can't output over UART, or doesn't have rolling logs, etc.
    It's impossible to please everyone and fulfill every possible requirement but ALF aims to be a natural choice for any evironment.
    Every aspect of ALF will be configurable and configuration management will be natural and painless.

## Why Another Logging Framework

Debugging embedded systems can be tricky, especially when your dealing with interrupts and timing.
Clogging up the runtime with expensive printf formatting can disrupt the timing in such a way
that the bug your chasing will no longer present itself. In these cases we have to get creative
and use some sort of in-memory binary logger, or toggle a digital output and take measurements with an oscilloscope. These methods, while effective, are tedious and not an ideal option when dealing with devices deployed in the field.
As a programmer, I'm lazy and because I'm lazy I don't want to spend time messing about with instrumentation. I'd rather just write a printf and call it a day.
ALF aims to please the lazy programmer like myself by deferring _most_ runtime costs associated with a printf to a later time.
We do this by tracing data out of the embedded target in binary and do the formatting on a powerful host computer where overhead doesn't matter.

## How it works

- Client's write printf style log statements in their code: `ALF_LOG("Temperature: %d", temp);`
- As a pre-compile step we compute a hash of each format string and replace the log statement with something like: `ALF_LOG_IMPL(0xF1F1F1, temp)`.
- Computing hashes should mask against module names so we can turn off logging/filter against a specific module
- During the pre-compile step we collect all the format strings and computed hashes into an ID list json file.
- At runtime, the logged data get's encoded into packets and sent out over the wire/air/morse code
- At the host side, the ALF tool will use the compiled id list and decode the data and printf with the appropriate string.

## Components

- ALF logger

  - Encoder and packet framing. Should use COBS since it's efficient, reliable, able to resync since the packet framing is unambiguous. But should allow the client to change the encoder if they want.
  - Immediate mode logging. encode and transfer data immediately.
  - Deferred mode. copy data over to a circular buffer then another thread encodes and transfers it.
  - Compiler and runtime log level support
  - Timestamp support. both system ticks and UTC timestamp.
  - Source code location encoding (file and line)
  - Module level support. Turn off logging for specific modules, filter logs by module.
  - Message encryption and PII redaction.
  - Uploadable format? Make interface able to uplaod files
  - File mode and stream mode? Stream over output channel and/or log to file? This should be seamless.
  - Rollover logs.
  - in-memory log support.
  - Logging statements essentially just need a unique ID and the optional data

- ALF Tool
  - Preprocessor. Compute unique hashes for each log string.
  - Versioning of ID list?
  - Hashes should have a module/file/level mask so output can be filtered.
  - Terminal color support. This can be applied on the filter.
  - communication channel agnostic. Should be able to read from a file, UART, RTT. etc.
