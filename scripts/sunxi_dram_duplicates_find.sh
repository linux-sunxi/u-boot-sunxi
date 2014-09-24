#!/bin/bash
#
# Tool to find identical dram definition files in board/sunxi.
#
# Author: Julian Calaby <julian.calaby@gmail.com>
#

TMPFILE=tempfile

find -wholename '*/sunxi/dram_*.c' \
       | while read r; do
		echo -n "$r:";
		# Rip out dram params
		cat $r \
			| sed -n '/^\s*static struct/,/^};/p' \
			| grep '^\s*\.' \
			| sed 's/,.*$//;s/^\s*//' \
			| sort \
			| while read a b c; do
				echo $a $b $((c));
			done \
			| tr -d '\n'; #concatenate
		echo;
	done \
	| tee $TMPFILE \
	| sed 's/^.*://' \
	| sort \
	| uniq -c \
	| grep -v '^\s*1\s' \
	| while read c r; do
		echo "Found a set of identical sunxi dram files:";
		grep -F "$r" $TMPFILE \
			| sed 's/:.*//';
		echo
	done;

rm -f $TMPFILE;
