#!/bin/bash
wget -O 0755 http://61.135.159.21/cgi-bin/weather?0755
wget -O 0751 http://61.135.159.21/cgi-bin/weather?0755
wget -O 0753 http://61.135.159.21/cgi-bin/weather?0755


iconv -f GB2312 -t UTF-8 0755 > 0755n
iconv -f GB2312 -t UTF-8 0751 > 0751n
iconv -f GB2312 -t UTF-8 0753 > 0753n

