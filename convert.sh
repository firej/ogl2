#!/bin/bash
# Convert text files recursively from cp1251 to utf-8
for i in `find ./ -type f -name '*.h'`; do
iconv -f cp1251 -t utf-8 $i >> $i.utf
mv $i.utf $i
echo "Convert " $i
done

