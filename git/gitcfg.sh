#!/bin/bash
git config --global user.name "chen dong"
git config --global user.email "china.gd.sz.cd@gmail.com"
git config --global core.editor "vim"
git config --global color.status "auto"
git config --global color.branch "auto"
git config --global color.diff "auto"
git config --global color.ui "true"
git config --global color.pager "true"
git config --global color.branch.current "yellow reverse"
git config --global color.branch.local "yellow"
git config --global color.branch.remote "green"
git config --global color.diff.meta "yellow bold"
git config --global color.diff.frag "magenta bold"
git config --global color.diff.old "red bold"
git config --global color.diff.new "green bold"
git config --global color.status.added "yellow"
git config --global color.status.changed "green"
git config --global color.status.untracked "cyan"
#gitmeld是一个脚本文件
git config --global diff.external gitmeld
##!/bin/sh
#meld $2 $5
