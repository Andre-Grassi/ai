#!/bin/bash

# Update sudo credentials first (so the background server doesn't get stuck)
sudo -v

# Start the server in the background
sudo ./start_server.sh &

# Give the server a moment to start up (Important!)
echo "Starting server..."
sleep 2

# Start the clients silently
../bin/adugo_main o 50 > /dev/null 2>&1 &
../bin/adugo_main c 50 > /dev/null 2>&1 &

# Keep this script running as long as the games are running
wait