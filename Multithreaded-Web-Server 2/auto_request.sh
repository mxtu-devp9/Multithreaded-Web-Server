#!/bin/bash

SERVER_IP=${1:-127.0.0.1}

echo "======================================"
echo " Continuous Auto Request Demo Started "
echo "======================================"
echo "Server IP: $SERVER_IP"
echo "Press CTRL + C to stop"
echo ""

COUNT=1

while true
do
  echo "---------- Round $COUNT ----------"

  curl -s "http://$SERVER_IP:8080/data?name=Rishabh&type=Normal" > /dev/null &
  curl -s "http://$SERVER_IP:8080/client-status?name=Avi&type=Normal" > /dev/null &
  curl -s "http://$SERVER_IP:8080/file?name=Abhi&type=Normal" > /dev/null &

  wait

  echo "Normal clients sent requests"

  if (( COUNT % 5 == 0 ))
  then
    echo "Sending PRIORITY request..."
    curl -s "http://$SERVER_IP:8080/pause?name=Inder&type=Priority" > /dev/null
    echo "Priority request completed"
  fi

  COUNT=$((COUNT + 1))

  sleep 2
done