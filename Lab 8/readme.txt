					Lab 2 

Group Members:

130050007	:	Vaibhav Bhosale
130050046	:	Dibyendu Mondal

We are using 2 laptops. 1 Lenovo laptop having Ubuntu 15.04 x86_64 architecture, 8GB RAM. The 2nd one, a DELL laptop having Ubuntu 15.04 x86_64 architecture, 4GB RAM. Our setup has both these laptops connected to a LAN.

To compile our code run make.
To run server(port say 5000):
./server-mt 5000 5 500

To run client(say IP 10.7.160.48, port 5000, threads 5, experiment duration 120s, sleep time 0s, fixed file)
./multi-client 10.7.160.48 5000 5 120 0 fixed