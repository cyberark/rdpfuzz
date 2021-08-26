#!/bin/bash
SERVICE="xfreerdp"
while true
do
    sleep 1
    if pgrep -x "$SERVICE" >/dev/null
    then
        echo "."
    else
        echo "not"
    fi
done