#!/bin/bash
/opt/hub/configs/html/mail_init.sh

#参数接指定的目录
/opt/hub/configs/html/mail_handle.sh /home/chend/114/usr/local/.cfg/.checkout

#参数接指定的目录的所有目录，目录1，目录2，目录3  ...
/opt/hub/configs/html/mail_post.sh /home/chend/114/usr/local/.cfg/.checkout
