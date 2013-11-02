#!/usr/bin/expect -f
spawn -ignore HUP /usr/bin/sshfs chend@18.8.8.115:/  /home/chend/250 -o sshfs_sync -o allow_other,kernel_cache,hard_remove
expect {
	"*password*" { send "12345678\r" }
}
expect eof


spawn -ignore HUP /usr/bin/sshfs chend@18.8.10.114:/  /home/chend/114 -o sshfs_sync -o allow_other,kernel_cache,hard_remove
expect {
	"*password*" { send "123456\r" }
}
expect eof
