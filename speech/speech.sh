#!/bin/bash
  say() { /usr/bin/mplayer -ao alsa -really-quiet -noconsolecontrols "http://translate.google.com/translate_tts?ie=UTF-8&tl=zh-CN&q=$*"; }
    say $*
    echo $?
#    http://translate.google.com/translate_tts?tl=zh-CN&q=
#      chmod u+x speech.sh

#        ./speech.sh Look Dave, I can see you're really upset about this.
