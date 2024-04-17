#!/bin/bash

# Compile server.c
gcc server.c -o server

# Check for processes listening on port 5002
PID=$(lsof -t -i:5002)

# Kill the process if it exists
if [ -n "$PID" ]; then
    echo "Killing process $PID"
    kill -9 "$PID"
fi

# Run the server
./server
