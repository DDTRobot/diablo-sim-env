#!/bin/bash

# Start webots-sim in the background
cd /opt/webots-sim-projects/sim-webots/diablo_A1/controllers/diablo_webots/
./diablo_webots > run_log.txt 2>&1 &

# Start supervisord to manage other processes
/usr/bin/supervisord -c /etc/supervisor/conf.d/supervisord.conf
