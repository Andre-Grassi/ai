# You will pobably want to run this without sudo. To do that,
# type `sudo visudo` and add the following line:
# `your_username ALL=(ALL) NOPASSWD: /full/path/to/your/start_server.sh`

redis-server server/redis.conf &
sleep 0.5

# Start controller
./server/controlador o 127.0.0.1 50 0
sudo pkill -9 redis-server