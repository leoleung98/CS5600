#!/bin/bash

# Compile the client
#gcc -o client client.c


# Wait for the server to start
sleep 1


# Test the "fget" command with "PUT" option
./client 127.0.0.1 2000 << EOF
fget PUT test.txt /home/leoleung/Desktop/course/CS5600/Practicum2/local.txt
QUIT
EOF

# Test the "fget" command with "PUT" option
./client 127.0.0.1 2000 << EOF
fget PUT /home/leoleung/Desktop/course/CS5600/Practicum2/local.txt
QUIT
EOF

# Test the "fget" command with "GET" option
./client 127.0.0.1 2000 << EOF
fget GET /tmp/test.txt /home/leoleung/Desktop/course/CS5600/Practicum2/local2.txt
QUIT
EOF

# Test the "fget" command with "GET" option
./client 127.0.0.1 2000 << EOF
fget GET /tmp/test.txt
QUIT
EOF

# Test the "fget" command with "MD" option
./client 127.0.0.1 2000 << EOF
fget MD /tmp/test-dir
QUIT
EOF

# Test the "fget" command with "MD" option
./client 127.0.0.1 2000 << EOF
fget MD /newFolder/show.txt
QUIT
EOF

# Test the "fget" command with "RM" option
./client 127.0.0.1 2000 << EOF
fget RM /newFolder/show.txt
QUIT
EOF

# Test the "fget" command with "RM" option
./client 127.0.0.1 2000 << EOF
fget RM /tmp/test-dir
QUIT
EOF

# Test the "fget" command with "INFO" option
./client 127.0.0.1 2000 << EOF
fget INFO /tmp/test.txt
QUIT
EOF

# Shut down the server
./client 127.0.0.1 2000 << EOF
SHUTDOWN
EOF
