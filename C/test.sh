#!/bin/bash

# Define the request headers
request="GET / HTTP/1.0\r\n"
# request+="Host: localhost:9000\r\n"
request+="If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT\r\n"
request+="\r\n"  # Empty line indicating the end of headers

# Use Telnet to send the request to localhost on port 9000
(echo -ne "$request"; sleep 1) | telnet localhost 9000
