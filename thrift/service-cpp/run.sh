pkill service-cpp
sleep 1
./service-cpp --type 0 --port 9090 --dataset parts_lookup.small &
./service-cpp --type 1 --port 9091 --dataset parts_lookup.small &
./service-cpp --type 1 --port 9092 --dataset parts_lookup.small &
