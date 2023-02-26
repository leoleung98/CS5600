# Makefile for BoundedBuffer and BoundedBufferTest
BoundedBufferFile = BoundedBuffer.java
BoundedBufferTestFile = BoundedBufferTest.java

# Flags to pass to the Java compiler
JFLAGS = -g

# Rule to build all targets
all: BoundedBufferTest

# Rule to build the BoundedBuffer class
BoundedBuffer: $(BoundedBufferFile)
	javac $(JFLAGS) $(BoundedBufferFile)

# Rule to build the BoundedBufferTest class
BoundedBufferTest: $(BoundedBufferTestFile) BoundedBuffer
	javac $(JFLAGS) $(BoundedBufferTestFile)

# Rule to run the BoundedBufferTest class
run: BoundedBufferTest
	java BoundedBufferTest

# Rule to clean up generated files
clean:
	rm -f *.class