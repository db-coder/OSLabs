Lab-11 Report

Group Members:

130050007	:	Vaibhav Bhosale
130050046	:	Dibyendu Mondal

We did the following changes in the Makefile:

Added the flags: -lcrypto -lssl
to LDADD and FUSE_LIBS variables

We did the following changes in the bbfs.c file:

We used AES encryption algorithm from the openssl library.

In the bb_read function:
First we read the whole file, then divided it into chunks of AES_BLOCK_SIZE and decrypted every chunk seperately and stored it in buf.

In the bb_write function:
First we divide buf into chunks of AES_BLOCK_SIZE and then encrypted each chunk seperately, stored it into buf_new and then wrote buf_new to the file.

Testing:

First we created mountdir and rootdir.
Case 1: Using simple cipher
../src/bbfs rootdir/ mountdir/ 0
Case 2: Using AES
../src/bbfs rootdir/ mountdir/ 1
Then we wrote a file in the mountdir, say: cat ../Makefile > foo
Now, if we open the file from mountdir, it's decrypted but if we open from the rootdir it's encrypted.

Note that this algorithm works for reading and writing the file and not if we edit a file, as AES works for AES_BLOCK_SIZE only, so modifying a file whose size is not a multiple of AES_BLOCK_SIZE will give pains.

We also tried simple cipher, in which instead of doing all these things we just replace buf[i] with buf[i]+1 in case of writing and buf[i] with buf[i]-1 in case of reading. This works in case of editing a file too.	