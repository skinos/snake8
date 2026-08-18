## tmptools@tty2http — Serial to HTTP bridge example

### Overview

Bridge the `uart@tty` serial port to an example HTTP server. Serial bytes are collected into a frame, posted over HTTP, and the HTTP response body is written back to the same port.
- Open `uart@tty` (`ttydev`, or `/dev/ttyS1` when that attribute is empty)
- Apply compile-time line settings (speed, databit, stopbit, parity, flow)
- POST each frame with libcurl to `http://192.168.8.250:8080/uart` as `application/octet-stream`
- Write the HTTP response body back to the serial port
    > Do not bind another `drvcom` on the same port while this example holds the device


### Concepts

**Compile-time parameters**

Line settings, framing, and the HTTP URL are macros (`TTY_SPEED`, `TTY_DATABIT`, `TTY_STOPBIT`, `TTY_PARITY`, `TTY_FLOW`, `TTY_FRAME_MS`, `TTY_FRAME_MAX`, `HTTP_HOST`, `HTTP_PORT`, `HTTP_PATH`). Rebuild after changing them.

**Serial frame**

Bytes are gathered until `TTY_FRAME_MS` milliseconds of idle, or `TTY_FRAME_MAX` bytes. That buffer is one HTTP POST.

**Example HTTP server**

Each frame is POSTed to `http://192.168.8.250:8080/uart`. The body of the HTTP reply is written to the serial port. An echo server on that URL makes a round-trip test.


### API Reference

#### Management APIs

+ `setup[]` **start the serial-to-HTTP bridge**
    - failed return tfalse
    - succeed return ttrue
    - Starts the long-running `service` process

    Example, start the example bridge
    ```shell
    tmptools@tty2http.setup
    ttrue
    ```

+ `shut[]` **stop the serial-to-HTTP bridge**
    - failed return tfalse
    - succeed return ttrue

    Example, stop the example bridge
    ```shell
    tmptools@tty2http.shut
    ttrue
    ```

+ `service` **run the serial-to-HTTP loop**
    - failed return tfalse
    - succeed return tfalse
    - Started by `setup`; not intended for direct invocation
    - Opens the uart@tty device and posts each serial frame to the example HTTP server
