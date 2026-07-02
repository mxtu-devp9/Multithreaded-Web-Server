#!/bin/bash
SERVER_IP=${1:-127.0.0.1}
COUNT=1

echo "Continuous Auto Request Demo Started"
echo "Server IP: $SERVER_IP"
echo "Press CTRL + C to stop"

while true
do
  echo "Round $COUNT"
  curl -s "http://$SERVER_IP:8080/data?name=Rishabh&type=Normal" > /dev/null &
  curl -s "http://$SERVER_IP:8080/client-status?name=Avi&type=Normal" > /dev/null &
  curl -s "http://$SERVER_IP:8080/file?name=Abhi&type=Normal" > /dev/null &
  wait

  if (( COUNT % 5 == 0 )); then
    echo "Sending priority request"
    curl -s "http://$SERVER_IP:8080/pause?name=Inder&type=Priority" > /dev/null
  fi

  COUNT=$((COUNT + 1))
  sleep 2
done
