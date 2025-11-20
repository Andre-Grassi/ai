redis-server server/redis.conf &
sleep 0.5

# Start controller
./server/controlador o 127.0.0.1 50 0