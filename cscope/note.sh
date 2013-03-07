find `pwd` -name "*.[ch]" -o -name "*.cpp" -o name "*.java" > cscope.files
cscope -bkq -i cscope.files
ctags -R
