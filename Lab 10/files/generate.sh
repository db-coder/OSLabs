for (( i = 0; i < 25; i++ )); do
	dd if=/dev/urandom of=foo$i.txt bs=10M count=1
done