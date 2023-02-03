# SampleCppCode
A basic serializer/deserializer example in the form of a market data generator/processor built on top of Bazel. Generates random trades and quotes to a file, and then processes file and writes ONLY trades to output.

Things to look for:
- State Machine Design for both the generator and processor
- Buffered reads/writes
- Use of alignas() for cache coherent programming
- Use of static assert to enforce invariants
- Decent testing coverage / error checking
- Unnecessarily materializing objects on the stack instead of writing/reading directly from streams
- General Modern C++ Features (std::byte vs char)
- Performance

Possible extensions:
- Logging (Multiple Levels)
- Thread Safety / Multiple Concurrent Streams
- Read / Write optimizations based on hardware
- Creating config files 
