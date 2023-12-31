# RC Library OpCode Documentation
## Representation

The internal data type used for opcodes is `unsigned char`, exposed under the type alias `byte`. On most systems, this is a one-byte datatype with and the range of valid values is `[0, 256)` or `0x00` to `0xFF`.

## Bitmasks

To make it easier to group the opcodes and verify which commands correspond to which modules, the values of the opcodes are chosen such that the leading bits identify the command category and the lowest bits are the commands themselves.

|| Bit 8 | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 |
|-- |---|---|---|---|---|---|---|---|
| Decimal value | 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |
| Hex value | `0x80` | `0x40` | `0x20` | `0x10` | `0x08` | `0x04` | `0x02` | `0x01` |
| Interpretation | Error? | Opcode | identification | bits | | Opcode | bits | |

The eighth bit is only set in responses that indicate an error. The maximum value for a command opcode is `0x07`.

The bitmask space thus allows for 16 different command categories with 8 total different commands each. There is also space for 16 possible response opcodes, of which half indicate errors.
