for (( i = 1165; i < 8000; i++ )); do
	dd if=/dev/zero of=foo$i.txt bs=2097152 count=1
done