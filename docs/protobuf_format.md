#### Protocol Buffers Serialization

Protocol Buffers (Protobuf) is a free and open-source cross-platform data format used to serialize structured data. It is useful in developing programs to communicate with each other over a network or for storing data. Protocol Buffers provide a serialization format for packets of typed, structured data that are up to a few megabytes in size. The format is suitable for both ephemeral network traffic and long-term data storage. This document describes the protocol buffer wire format, which defines the details of how your message is sent on the wire and how much space it consumes on disk. You probably don’t need to understand this to use protocol buffers in your application, but it’s useful information for doing optimizations.

Variable-width integers, or varints, are at the core of the wire format. They allow encoding unsigned 64-bit integers using anywhere between one and ten bytes, with small values using fewer bytes. Each byte in the varint has a continuation bit that indicates if the byte that follows it is part of the varint. This is the most significant bit (MSB) of the byte (sometimes also called the sign bit). The lower 7 bits are a payload; the resulting integer is built by appending together the 7-bit payloads of its constituent bytes.

#### Base 128 Varints (LEB128) Encoding Technique

To overcome the problem of representing varying length integer values, the **Base 128 Varints** technique is used. This technique eliminates the need for a delimiter byte and instead uses a single bit for this purpose. The advantage of this method is that it can save a significant amount of storage while accommodating any number of bytes associated with an integer value.

This encoding technique is part of the family of variable-length encodings, and it is commonly known as **LEB128 (Little Endian 128)**. It finds applications in various formats like DWARF file format, WebAssembly, and Google Protobuf to efficiently represent integer values using the smallest possible number of bytes.

The process of encoding an integer to LEB128 is relatively straightforward and consists of the following steps:

1. Convert the number to its binary representation.
2. Group the binary bits into sets of 7 bits, leaving the 8th bit empty in each group.
3. For the least significant bytes (LSB), fill up the 8th bit of each byte with a value of 1.
4. Fill the remaining bits of the most significant byte (MSB) with 0.
5. Reverse the order of the bytes.
6. Now, you have the LEB128 encoded binary representation.

When reading the byte sequence:

1. Read one byte from the sequence of bytes.
2. Check if the 8th bit of that byte is set to 1, and if so, keep it in a buffer, then repeat step 1.
3. When reading a byte with the 8th bit set to 0, it indicates that all bytes associated with one integer value are complete.
4. Reverse the order of the bytes in the buffer.
5. Remove the 8th bit of each byte in the buffer.
6. Join the remaining 7 bits of each byte together.
7. Finally, you will obtain your original integer value.

#### First Byte of Protocol Buffers Serialized File

In a Protocol Buffers (protobuf) serialized file, the first byte represents the "field number" and the "wire type" of the first field present in the message. The structure of the first byte (varint) is as follows:

```
Field Number (5 bits) | Wire Type (3 bits)
```

- The "Field Number" indicates the tag or identifier for the field in the protobuf message. It can take values from 1 to 536,870,911 (2^29-1).
- The "Wire Type" defines the encoding format of the data and can have the following values:
  - 0: Varint (int32, int64, uint32, uint64, sint32, sint64, bool, enum)
  - 1: 64-bit fixed (fixed64, sfixed64, double)
  - 2: Length-delimited (string, bytes, embedded messages, packed repeated fields)
  - 3: Start group (deprecated, not used anymore)
  - 4: End group (deprecated, not used anymore)
  - 5: 32-bit fixed (fixed32, sfixed32, float)

The actual value of the first byte depends on the specific field number and wire type of the first field in your particular protobuf message. Each field in the serialized message will be encoded with a combination of the varint for the field number and wire type, followed by the actual field data in the format defined by the wire type.

```c
// C code to demonstrate encoding the field number and wire type into the first byte of a protobuf message

#include <stdio.h>

int main() {
    // Example of encoding a field number and wire type into the first byte
    int fieldNumber = 7;  // Field number of the first field in the message
    int wireType = 2;     // Wire type indicating length-delimited data

    // Combine the field number and wire type into the first byte
    unsigned char firstByte = (fieldNumber << 3) | wireType;

    // Print the binary representation of the first byte
    printf("First Byte in Binary: ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (firstByte >> i) & 1);
    }
    printf("\n");

    return 0;
}
```

Note: The C code provided here is a simple example to demonstrate the concept of encoding the field number and wire type into the first byte. In a real-world protobuf implementation, a library or framework would typically handle the serialization and deserialization process.

for example 0x08 will be 00001000, so 00001 for field number and 000 for wire format, so its means
`field 1 : Varint `

related to onnx, and ModelProto

```
message ModelProto {
  // The version of the IR this model targets. See Version enum above.
  // This field MUST be present.
  optional int64 ir_version = 1;

  // The OperatorSets this model relies on.
  // All ModelProtos MUST have at least one entry that
  // specifies which version of the ONNX OperatorSet is
  // being imported.
  //
  // All nodes in the ModelProto's graph will bind against the operator
  // with the same-domain/same-op_type operator with the HIGHEST version
  // in the referenced operator sets.
  repeated OperatorSetIdProto opset_import = 8;

  // The name of the framework or tool used to generate this model.
  // This field SHOULD be present to indicate which implementation/tool/framework
  // emitted the model.
  optional string producer_name = 2;

  // The version of the framework or tool used to generate this model.
  // This field SHOULD be present to indicate which implementation/tool/framework
  // emitted the model.
  optional string producer_version = 3;

  // Domain name of the model.
  // We use reverse domain names as name space indicators. For example:
  // `com.facebook.fair` or `com.microsoft.cognitiveservices`
  //
  // Together with `model_version` and GraphProto.name, this forms the unique identity of
  // the graph.
  optional string domain = 4;

  // The version of the graph encoded. See Version enum below.
  optional int64 model_version = 5;

  // A human-readable documentation for this model. Markdown is allowed.
  optional string doc_string = 6;

  // The parameterized graph that is evaluated to execute the model.
  optional GraphProto graph = 7;

  // Named metadata values; keys should be distinct.
  repeated StringStringEntryProto metadata_props = 14;

  // Training-specific information. Sequentially executing all stored
  // `TrainingInfoProto.algorithm`s and assigning their outputs following
  // the corresponding `TrainingInfoProto.update_binding`s is one training
  // iteration. Similarly, to initialize the model
  // (as if training hasn't happened), the user should sequentially execute
  // all stored `TrainingInfoProto.initialization`s and assigns their outputs
  // using `TrainingInfoProto.initialization_binding`s.
  //
  // If this field is empty, the training behavior of the model is undefined.
  repeated TrainingInfoProto training_info = 20;

  // A list of function protos local to the model.
  //
  // Name of the function "FunctionProto.name" should be unique within the domain "FunctionProto.domain".
  // In case of any conflicts the behavior (whether the model local functions are given higher priority,
  // or standard opserator sets are given higher priotity or this is treated as error) is defined by
  // the runtimes.
  //
  // The operator sets imported by FunctionProto should be compatible with the ones
  // imported by ModelProto and other model local FunctionProtos.
  // Example, if same operator set say 'A' is imported by a FunctionProto and ModelProto
  // or by 2 FunctionProtos then versions for the operator set may be different but,
  // the operator schema returned for op_type, domain, version combination
  // for both the versions should be same for every node in the function body.
  //
  // One FunctionProto can reference other FunctionProto in the model, however, recursive reference
  // is not allowed.
  repeated FunctionProto functions = 25;
};
```

Field 1 is defined as `optional int64 ir_version = 1`, and it is represented as a varint. To retrieve the value from the following byte(s), for example, 0x07 (00000111) with the most significant bit (MSB) not set, it defines the version 7 of the model

```
  // IR VERSION 7 published on May 8, 2020
  // - Add support to allow function body graph to rely on multiple external opreator sets.
  // - Add a list to promote inference graph's initializers to global and
  //   mutable variables. Global variables are visible in all graphs of the
  //   stored models.
  // - Add message TrainingInfoProto to store initialization
  //   method and training algorithm. The execution of TrainingInfoProto
  //   can modify the values of mutable variables.
  // - Implicitly add inference graph into each TrainingInfoProto's algorithm.
  IR_VERSION_2020_5_8 = 0x0000000000000007;
```

Following the same logic, if we have 0x12 (00010010) as the following byte, it may indicate Field 2, which is of Length-delimited type `optional string producer_name = 2`. Subsequently, we encounter the value 0x0C (00001100), which represents the length 12. Thus, we can read 12 bytes from the stream to obtain the value of the producer_name field.

And so on..

Here are the links to the sources:

[Protocol Buffers - Wikipedia](https://en.wikipedia.org/wiki/Protocol_Buffers)<br>
[Encoding | Protocol Buffers Documentation](https://protobuf.dev/programming-guides/encoding/)<br>
[Overview | Protocol Buffers Documentation](https://protobuf.dev/overview/)<br>
