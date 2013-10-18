#!/bin/bash
echo "first para is group, second para is dir or file"
if [ $# != 2 ]; then
echo "first para is user, second para is group"
exit
fi
echo "first para is user, second para is group"
echo "sudo chgrp $1 $2"
sudo chgrp $1 $2
#sudo chgrp audio /var/lib/mpd/music
