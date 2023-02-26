BoundedBuffer, BoundedBufferTest and Makefile
This repository contains Java code for a Bounded Buffer implementation, a test for it, and a Makefile to compile and run them.

BoundedBuffer.java
BoundedBuffer.java contains the implementation of the Bounded Buffer, a data structure that provides a fixed-size queue with blocking operations that ensure thread safety.

The implementation uses the Lock and Condition classes from the java.util.concurrent.locks package to provide mutual exclusion and synchronization among threads.

To use the Bounded Buffer, create an instance with a specified capacity, and call deposit to add an element to the buffer, and fetch to remove an element from the buffer. Both methods are blocking, and will wait until the buffer has space or elements, respectively.


BoundedBufferTest.java
BoundedBufferTest.java contains a test for the Bounded Buffer implementation, using three threads that randomly produce and consume strings from the buffer.

To run the test, create an instance of BoundedBuffer with a capacity of 10, and create three instances of ProducerConsumer with the buffer as an argument. Then start the threads and wait for them to finish.

The test also prints the remaining elements in the buffer after the threads have finished, and reports if the buffer is empty.

Makefile
Makefile contains rules to compile and run the Java classes. The all rule builds the BoundedBufferTest class, which depends on the BoundedBuffer class. The run rule runs the BoundedBufferTest class. The clean rule removes the generated .class files.

To use the Makefile, simply run "make" to build and "make run" to run the test. Use make "make clean" to remove generated files.
