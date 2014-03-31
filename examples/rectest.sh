#!/bin/bash
make -f gnu.make EXAMPLE=rectest

ybug 192.168.0.11 << EOF

boot /home/eroe/Desktop/Work/spinnaker_package/tools/scamp-2.boot
sleep 1

iptag .
p2pc 2 2 

sp 0 0
sload rectest.aplx f5000000
#as f5000000 2 

sp 1 0
sload rectest.aplx f5000000

sleep 1

as f5000000 2 
#as f5000000 6 
#as f5000000 1fffe
#as f5000000 e

sp root

EOF
