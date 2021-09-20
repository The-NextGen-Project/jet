// Before attempting to read this file, please take the time to become familiar
// with x64 instruction encoding. For a quick explanation read the following:
//
// Encoding:
// There are three main types of encoding in x64:
//    1. Mod RM
//    2. SIB
//    3. Rex
// Section 1: Mod RM
// The Mod RM byte is the most common encoding in instructions and is used in
// most instructions. It is encoded as follows:
// | MOD | REG | RM |
// Mod values:
// 00 - SIB no displacement, displacement only, or register indirect addressing
//    Examples:
//      mov rax, [32] -> Here mod is 00 because we are only using displacement
//      mov rax, [rbp + (rcx * 1)] -> Mod is 00 because we require a SIB byte
//      but no displacement is required.
// 01 - Memory Address displacement of 1 byte
// 10 - Memory Address displacement of 4 bytes
// 11 - Register addressing mode (Register operands only)
//
// Reg values:
// In x86 register values are usually from 1 to 8, but in x64 they
// are from 1 to 15 for each argument size. What do we mean? We mean the bits
// that the register can hold. For example, rax is a 64-bit register, while
// eax is a 32-bit register, and so on. The significant part of the register
// value is in the last 3 bits which are placed in the center of the ModRM byte.
// For example, the register 'rdx' has last 3 bits in binary as 010.
//
// Since x64 Registers go up to 15, the new registers that are added are from
// R8-R15. These registers can also be accessed in their respective lower
// argument sized registers:
//    'N' represents a value from 8 to 15
//    RN - 64 bit
//    RND - 32 bit
//    RNW - 16 bit
//    RNB - 8 bit
// These registers require are special and require instructions to have a
// rex-byte. Not only these registers but special 8-bit registers AH, CH, DH,
// and BH also require a rex-byte when encoded.
//
// RM Values:
// These values represent the second operand to a binary instruction or a
// single operand to a unary instruction. In the case of unary instructions
// like NOT, they have an extra opcode value that replaces the REG category
// in the Mod RM byte.
//
//
// Section 2: SIB
// The SIB byte stands for (Scale Index Base) which is a byte required in
// certain x64 addressing encoding. This byte does not always get encoded
// when passing a memory address as an operand, but its presence effects the
// rex-byte. The encoding for this byte is the same as Mod RM, with the
// difference in the values of each section.
//
// Scale values:
// 00 - Represents (index * 1)
// 01 - Represents (index * 2)
// 10 - Represents (index * 4)
// 11 - Represents (index * 8)
//
//
// Index values:
// These values are also based off last 3-bits of a register. NOTE: When a
// SIB byte is encoded, a Mod RM byte is also encoded with values in relation to
// the memory address. In the case that a SIB byte is encoded, the value of
// the SP register takes the place of the RM slot in the Mod RM byte.
//
// IMPORTANT: The default base and index registers are BP and SP respectively,
// unless specified in the memory address.
//
// IMPORTANT: SP register is an illegal value for index in SIB encoding
//
// Base values:
// Same as index values, except SP registers are allowed.
//
//
// Section 3: REX
// The rex byte is usually the first byte in an instruction main specifying
// whether the instruction's operation is 64-bit or not. However, the REX
// byte specifies more than just that. Encoding is as below:
// | 0b1000 | W | R | X | B |
// Values of W, R, X, and B are single bit values.
// W - Specifies 64-bit operation
// R - REG value in MOD RM
// X - INDEX value in SIB byte
// B - RM value in MOD RM
// NOTE: When dealing with 16-bit values, even before the rex-byte, Legacy
// Prefix value 0x66 should be written to the buffer.

#include <nextgen/jet/jet-x64-assembler.h>

using namespace nextgen::jet::x64;

// Certain unary instructions (1 operand) have patterns pertaining to the
// way that they are encoded, due to this, a macro has been created to simplify
// the amount of code written and redundancy for slight changes in the code.
#define unary_instruction(name, op1, op2, extra_opcode)\
void Assembler::name##b(const Register reg) {\
if (rex_needed2(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));\
write(op1, EncodeModRM<0b11, extra_opcode>(reg));\
}\
void Assembler::name##w(const Register reg) {                            \
write(0x66);          \
if (rex_needed(reg)) write(REX_PREFIX | REX_B);\
write(op2, EncodeModRM<0b11, extra_opcode>(reg));\
}\
void Assembler::name##d(const Register reg) {\
if (rex_needed(reg)) write(REX_PREFIX | REX_B);\
write(op2, EncodeModRM<0b11, extra_opcode>(reg));\
}\
void Assembler::name##q(const Register reg) {\
if (rex_needed(reg)) write(REX_PREFIX | REX_W | REX_B);\
else write(REX_PREFIX | REX_W);\
write(op2, EncodeModRM<0b11, extra_opcode>(reg));\
}\
void Assembler::name##b(const MemoryAddress &addr) {                     \
rex_optional_rr3(addr.base, addr.index);                              \
write(op1);                                          \
write_address(addr, (const Register)(extra_opcode)); \
}                                                      \
void Assembler::name##w(const MemoryAddress &addr) {                     \
rex_optional_rr2(addr.base, addr.index);                              \
write(op2);                                          \
write_address(addr, (const Register)(extra_opcode)); \
} \
void Assembler::name##d(const MemoryAddress &addr) {                     \
rex_optional_rr3(addr.base, addr.index);                              \
write(op2);                                          \
write_address(addr, (const Register)(extra_opcode)); \
} \
void Assembler::name##q(const MemoryAddress &addr) {                     \
rex_optional_rr4(addr.base, addr.index);                              \
write(op2);                                          \
write_address(addr, (const Register)(extra_opcode)); \
}  \

#define binary_instruction(name, o1, o2, m1, m2) \
void Assembler::name##b(const Register dest, const Register src) { \
if (rex_needed2(dest) || rex_needed2(src)) write(REX_PREFIX | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);\
write(o1, EncodeModRM<0b11> (src, dest));\
} \
void Assembler::name##w(const Register dest, const Register src) {  \
if (rex_needed(dest) || rex_needed(src)) write(0x66, REX_PREFIX | (src & 0b1000)>> 1 | (dest & 0b1000) >> 3); \
else write(0x66); \
write(o2, EncodeModRM<0b11> (src, dest));                                         \
}                      \
void Assembler::name##d(const Register dest, const Register src) { \
if (rex_needed(dest) || rex_needed(src)) write(REX_PREFIX | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);                                           \
write(o2, EncodeModRM<0b11>(src, dest));  \
}                          \
void Assembler::name##q(const Register dest, const Register src) { \
if (rex_needed(dest) || rex_needed(src)) write(REX_PREFIX | REX_W | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);                               \
else write(0x48); \
write(o2, EncodeModRM<0b11> (src, dest));\
}                                                \
void Assembler::name##b(const Register dest, const MemoryAddress &src) { \
rex_optional_rm1(dest, src); \
write(m1); \
write_address(src, dest); \
} \
\
void Assembler::name##w(const Register dest, const MemoryAddress &src) { \
rex_optional_rm2(dest, src); \
write(m2); \
write_address(src, dest); \
} \
\
void Assembler::name##d(const Register dest, const MemoryAddress &src) { \
rex_optional_rm3(dest, src); \
write(m2); \
write_address(src, dest); \
} \
\
void Assembler::name##q(const Register dest, const MemoryAddress &src) { \
rex_optional_rm4(dest, src); \
write(m2); \
write_address(src, dest); \
} \
\
void Assembler::name##b(const MemoryAddress &dest, const Register src) { \
rex_optional_rm1(src, dest);\
write(o1);\
write_address(dest, src);\
}\
\
void Assembler::name##w(const MemoryAddress &dest, const Register src) { \
rex_optional_rm2(src, dest);\
write(o2);\
write_address(dest, src);\
} \
\
void Assembler::name##d(const MemoryAddress &dest, const Register src) { \
rex_optional_rm3(src, dest); \
write(o2); \
write_address(dest, src); \
} \
\
void Assembler::name##q(const MemoryAddress &dest, const Register src) { \
rex_optional_rm4(src, dest); \
write(o2); \
write_address(dest, src); \
} \



#define binary_instruction_imm(name, AL_, AX_, extra_opcode) \
void Assembler::name##b(const Register reg, Imm8 imm) {                   \
if (rex_needed2(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3)); \
if (reg == AX) write(AL_, imm);                         \
else write(0x80, EncodeModRM<0b11>((const Register)(extra_opcode), reg), imm);\
}                                                            \
\
void Assembler::name##w(const Register reg, Imm8 imm) {                       \
write(0x66); \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                             \
write(0x83, EncodeModRM<0b11>((const Register)extra_opcode, reg), imm); \
}                                                            \
void Assembler::name##d(const Register reg, Imm8 imm) {                       \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                             \
write(0x83, EncodeModRM<0b11>((const Register)extra_opcode, reg), imm); \
}                                                           \
void Assembler::name##q(const Register reg, Imm8 imm) {                       \
if (rex_needed(reg)) write(REX_PREFIX | REX_W |(reg & 0b1000 >> 3));                                                       \
else write(0x48); \
write(0x83, EncodeModRM<0b11>((const Register)extra_opcode, reg), imm); \
}                                                \
void Assembler::name##w(const Register reg, Imm16 imm) {                      \
if (reg == AX) write(0x66, AX_, imm);                       \
else {                                                     \
write(0x66);                                            \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                         \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
} \
void Assembler::name##w(const Register reg, Imm32 imm) { \
if (reg == AX) write(AX_, imm);                       \
else {                                                        \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                         \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
} \
void Assembler::name##d(const Register reg, Imm16 imm) { \
if (reg == AX) write(AX_, imm);                       \
else {                                                        \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                         \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
}                                                         \
void Assembler::name##d(const Register reg, Imm32 imm) { \
if (reg == AX) write(AX_, imm);                       \
else {                                                       \
if (rex_needed(reg)) write(REX_PREFIX | (reg & 0b1000 >> 3));                                                         \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
}                                                           \
void Assembler::name##q(const Register reg, Imm16 imm) { \
if (reg == AX) write(0x48, AX_, imm);                       \
else {                                                       \
if (rex_needed(reg)) write(REX_PREFIX | REX_W | (reg & 0b1000 >> 3));                                                  \
else write(0x48); \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
}                                                            \
void Assembler::name##q(const Register reg, Imm32 imm) { \
if (reg == AX) write(0x48, AX_, imm);                       \
else {                                                       \
if (rex_needed(reg)) write(REX_PREFIX | REX_W | (reg & 0b1000 >> 3));                                                  \
else write(0x48); \
write(0x81, EncodeModRM<0b11>((const Register)extra_opcode,reg), imm);\
}\
\
}  \

#define floating_point_binary(name, float_prefix) \
void Assembler::name##ss(Register dest, Register src) { \
write(0xF3); \
rex_optional_rr3(src, dest); \
write(0x0F, float_prefix, EncodeModRM<0b11>(dest, src)); \
}\
void Assembler::name##ss(Register reg, const MemoryAddress &addr) { \
write(0xF3); \
rex_optional_rm3(reg, addr); \
write(0x0F, float_prefix); \
write_address(addr, reg); \
}                                                                \
void Assembler::name##sd(Register dest, Register src) {       \
write(0xF2);                                                   \
rex_optional_rr3(src, dest);                                   \
write(0x0F, float_prefix, EncodeModRM<0b11>(dest, src));                     \
} \
void Assembler::name##sd(Register reg, const MemoryAddress &addr) { \
write(0xF2); \
rex_optional_rm3(reg, addr);\
write(0x0F, float_prefix);\
write_address(addr, reg); \
} \

// [disp32]
MemoryAddress::MemoryAddress(uint disp32)
: has_disp32(true), mod(MOD_INDIRECT), disp(disp32) {}

// [base]
MemoryAddress::MemoryAddress(const Register base) : base(base), mod(MOD_INDIRECT) {
  has_sib = (base & DI) == SP;
  if ((base & DI) == BP) {
    // Encodes as [base+0x0]
    has_disp8 = true;
    mod = MOD_DISP8;
  }
}

// [base + disp8]
MemoryAddress::MemoryAddress(const Register base, ubyte disp8)
: base(base), mod (MOD_DISP8), disp(disp8) {
  has_sib = (base & DI) == SP;
  has_disp8 = true;
}

// [base + disp32]
MemoryAddress::MemoryAddress(const Register base, uint disp32)
: base(base), mod (MOD_DISP32), disp(disp32) {
  has_sib = (base & DI) == SP;
  has_disp32 = true;
}

// [base + (index * scale)]
MemoryAddress::MemoryAddress(const Register base, const Register index, SibScale scale)
: base(base), index(index), scale(scale), has_sib(true), mod(MOD_INDIRECT) {
  if ((base & DI) == BP) {
    mod = MOD_DISP8;
    has_disp8 = true;
  }
}

// [base + (index * scale) + disp8]
MemoryAddress::MemoryAddress(const Register base,
                             const Register index,
                             SibScale scale,
                             ubyte disp8)
                             : base(base), index(index), scale(scale),
                             mod(MOD_DISP8), has_disp8(true), disp(disp8),
                             has_sib(true) {}


                             // [base + (index * scale) + disp32]
                             MemoryAddress::MemoryAddress(const Register base,
                                                          const Register index,
                                                          SibScale scale,
                                                          uint disp32)
                                                          : base(base), index(index), scale(scale), mod(MOD_DISP32),
                                                          has_disp32 (true), disp(disp32), has_sib(true) {}


                                                          /// Encode ModRM (no values known)
                                                          static ubyte EncodeModRM(ubyte mod, const Register reg, const Register rm) {
  return (mod << 6) | ((reg & DI) << 3) | (rm & DI);
}

/// Encode ModRM (mod value known)
template<ubyte mod>
static ubyte EncodeModRM(const Register reg, const Register rm) {
  return (mod << 6) | ((reg & DI) << 3) | (rm & DI);
}

/// Encode ModRM (mod and reg value known)
template<ubyte mod, ubyte reg>
static ubyte EncodeModRM(const Register rm) {
  return (mod << 6) | ((reg & DI) << 3) | (rm & DI);
}

/// Check if a rex value is needed for a given register for
/// argument sizes that are (16, 32, 64) bit.
static bool rex_needed(const Register reg) {
  switch (reg) {
    case R8:
      case R9:
        case R10:
          case R11:
            case R12:
              case R13:
                case R14:
                  case R15:
                    return true;
                    default:
                      return false;
  }
}

/// This function is the same as the 'rex_needed', but the only difference
/// is that it checks for byte registers ah, ch, dh, bh when counting whether
/// a rex byte should be encoded. As per manual/reference, 8-bit registers
/// ah, ch, dh, and bh require a rex byte.
static bool rex_needed2(const Register reg) {
  switch (reg) {
    case R8:
      case R9:
        case R10:
          case R11:
            case R12:
              case R13:
                case R14:
                  case R15:
                    case SP: // AH
                    case BP: // CH
                    case SI: // DH
                    case DI: // BH
                    return true;
                    default:
                      return false;
  }
}

/// Allocate the assembler with given 'size' value.
Assembler::Assembler(const size_t size) {
  memory = (ubyte *) malloc(size);
}

/// Allocate the virtual page with given 'size' or give potential
/// executable permissions to allocated memory.
Assembler::Assembler(const size_t size, bool executable) {
  // TODO: Add mmap to the virtual memory allocation to allocate the memory
  // as executable
#ifdef NG_OS_WINDOWS
memory = (ubyte*) VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE,
                               executable ?
                               PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
#endif
}

/// Spill the memory buffer containing machine code bytes written
ubyte *Assembler::spill() const {
  return memory;
}

/// The number of bytes that have been written to the memory buffer
ulong Assembler::bytes() const {
  return used;
}

/// Write a byte to the memory buffer.
void Assembler::write(ubyte value) {
  memory[used] = value;
  used += sizeof(value);
}

/// Write a 16-bit value to the memory buffer.
void Assembler::write_16(ushort value) {
  write(ubyte(value));
  write(ubyte(value >> 8));
}

/// Write a 32-bit value to the memory buffer.
void Assembler::write_32(uint value) {
  write(ubyte(value));
  write(ubyte(value >> 8));
  write(ubyte(value >> 16));
  write(ubyte(value >> 24));
}

/// Write a 64-bit value to the memory buffer.
void Assembler::write_64(ulong value) {
  write(ubyte(value));
  write(ubyte(value >> 8));
  write(ubyte(value >> 16));
  write(ubyte(value >> 24));
  write(ubyte(value >> 32));
  write(ubyte(value >> 40));
  write(ubyte(value >> 48));
  write(ubyte(value >> 52));
}

/// Write an immediate 8-bit value.
void Assembler::write(Imm8 imm) {
  write(ubyte(imm.value));
}

/// Write an immediate 16-bit value.
void Assembler::write(Imm16 imm) {
  write_16(imm.value);
}

/// Write an immediate 32-bit value.
void Assembler::write(Imm32 imm) {
  write_32(imm.value);
}

/// Write an immediate 64-bit value.
void Assembler::write(Imm64 imm) {
  write_64(imm.value);
}

/// Write a memory address given the register destination
/// Reference: http://www.c-jump.com/CIS77/CPU/x86/lecture.html
void Assembler::write_address(const MemoryAddress &addr, const Register dest) {

  // Mod RM Byte Special Cases:
  // 1. Address has SIB with no displacement R/M is 0b100 (SP)
  // 2. Address has only displacement R/M is 0b101 (BP)
  write((addr.mod << 6) | (dest & DI) << 3 | (addr.has_sib ? SP : (addr.base & DI)));

  // Write SIB if it has one
  if (addr.has_sib)
    write((addr.scale << 6) | (addr.index  & DI) << 3 | addr.base);

  // Write Displacement if it has one
  if (addr.has_disp32) write_32(addr.disp);
  else if (addr.has_disp8) write(ubyte(addr.disp));
}

template<typename T, typename K>
void Assembler::write(T f, K s) {
  write(f), write(s);
}

template<typename T, typename K, typename L>
void Assembler::write(T f, K s, L t) {
  write(f), write(s), write(t);
}

template<typename T, typename K, typename L, typename Q>
void Assembler::write(T f, K s, L t, Q fr) {
  write(f), write(s), write(t), write(fr);
}

void Assembler::rex_optional_r1(const Register reg) {
  if (rex_needed2(reg))
    write(REX_PREFIX | (reg & 0b1000) >> 3);
}

/// Write optional rex-values for argument sizes of 16-bits.
/// 16-bit argument sizes require the legacy prefix 0x66.
/// If the register is R8-R15, it must have another rex byte
/// that is written.
void Assembler::rex_optional_r2(const Register reg) {
  write(0x66);
  if (rex_needed(reg))
    write(REX_PREFIX | (reg & 0b1000) >> 3);
}

/// Apply rex-byte to all operations that are not BYTE size.
void Assembler::rex_optional_r3(const Register reg) {
  if (rex_needed(reg))
    write(REX_PREFIX | (reg & 0b1000) >> 3);
}

/// Writes rex-values for 64-bit argument operations. This applies for
/// the long-mode of x86-- which is x64. It also integrates rex-prefix
/// with special register R8-R15.
void Assembler::rex_optional_r4(const Register reg) {
  if (rex_needed(reg))
    write(REX_PREFIX| REX_W | (reg & 0b1000) >> 3);
  else write(0x48);
}

/// Two const Register -- BYTE SIZE
void Assembler::rex_optional_rr1(const Register dest, const Register src) {
  if (rex_needed2(src) || rex_needed2(dest))
    write(REX_PREFIX | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);
}

/// Two const Register -- WORD SIZE
void Assembler::rex_optional_rr2(const Register dest, const Register src) {
  write(0x66);
  if (rex_needed(src) || rex_needed(dest))
    write(REX_PREFIX | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);
}

/// Two const Register -- DWORD SIZE
void Assembler::rex_optional_rr3(const Register dest, const Register src) {
  if (rex_needed(src) || rex_needed(dest))
    write(REX_PREFIX | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);
}

/// Two const Register -- QWORD SIZE
void Assembler::rex_optional_rr4(const Register dest, const Register src) {
  if (rex_needed(src) || rex_needed(dest))
    write(REX_PREFIX | REX_W | (src & 0b1000) >> 1 | (dest & 0b1000) >> 3);
  else write(0x48);
}

/// Memory Operation Rex Byte -- BYTE SIZE
void Assembler::rex_optional_rm1(const Register reg, const MemoryAddress &addr) {
  if (rex_needed2(reg) || rex_needed(addr.base) || rex_needed(addr.index))
    write(
      REX_PREFIX
      // R
      | (reg & 0b1000) >> 1
      // X
      | (addr.index & 0b1000) >> 2
      // B
      | (addr.base & 0b1000) >> 3);
}

/// Memory Operation Rex Byte -- WORD SIZE
void Assembler::rex_optional_rm2(const Register reg, const MemoryAddress &addr) {
  write(0x66);
  if (rex_needed(reg) || rex_needed(addr.base) || rex_needed(addr.index))
    write(
      REX_PREFIX
      // R
      | (reg & 0b1000) >> 1
      // X
      | (addr.index & 0b1000) >> 2
      // B
      | (addr.base & 0b1000) >> 3);
}

/// Memory Operation Rex Byte -- DWORD SIZE
void Assembler::rex_optional_rm3(const Register reg, const MemoryAddress &addr) {
  if (rex_needed(reg) || rex_needed(addr.base) || rex_needed(addr.index))
    write(
      REX_PREFIX
      // R
      | (reg & 0b1000) >> 1
      // X
      | (addr.index & 0b1000) >> 2
      // B
      | (addr.base & 0b1000) >> 3);
}

/// Memory Operation Rex Byte -- QWORD SIZE
void Assembler::rex_optional_rm4(const Register reg, const MemoryAddress &addr) {
  if (rex_needed2(reg) || rex_needed(addr.base) || rex_needed(addr.index))
    write(
      REX_PREFIX
      // W
      | REX_W
      // R
      | (reg & 0b1000) >> 1
      // X
      | (addr.index & 0b1000) >> 2
      // B
      | (addr.base & 0b1000) >> 3);
  else write(0x48);
}

//======================
// Unary Instructions
//======================
unary_instruction(inc, 0xFE, 0xFF, 0);
unary_instruction(dec, 0xFE, 0xFF, 1);
unary_instruction(not, 0xF6, 0xF7, 2);
unary_instruction(neg, 0xF6, 0xF7, 3);
unary_instruction(mul, 0xF6, 0xF7, 4);
unary_instruction(div, 0xF6, 0xF7, 6);
unary_instruction(idiv, 0xF6, 0xF7, 7);

//======================
// Binary Instructions
//======================
binary_instruction(add, 0x00, 0x01, 0x02, 0x03);
binary_instruction_imm(add, 0x04, 0x05, 0);

binary_instruction(sub, 0x28, 0x29, 0x2A, 0x2B);
binary_instruction_imm(sub, 0x2C, 0x2D, 5);

binary_instruction(and, 0x20, 0x21, 0x22, 0x23);
binary_instruction_imm(and, 0x24, 0x25, 4);

binary_instruction(xor, 0x30, 0x31, 0x32, 0x33);
binary_instruction_imm(xor, 0x34, 0x35, 6);

binary_instruction(cmp, 0x38, 0x39, 0x3A, 0x3B);
binary_instruction_imm(cmp, 0x3C, 0x3D, 7);

binary_instruction(or, 0x08, 0x09, 0x0A, 0x0B);
binary_instruction_imm(or, 0x0C, 0x0D, 1);

floating_point_binary(add, 0x58);
floating_point_binary(sub, 0x5C);
floating_point_binary(div, 0x5E);
floating_point_binary(mul, 0x59);

void Assembler::popq(const Register reg) {
  write(0x58+reg);
}

void Assembler::popw(const Register reg) {
  write(0x66, 0x58+reg);
}

void Assembler::pushq(const Register reg) {
  rex_optional_r4(reg);
  write(0x50 + (reg & DI));
}

void Assembler::pushw(const Register reg) {
  rex_optional_r2(reg);
  write(0x50 + (reg & DI));
}

void Assembler::movb(const Register reg, Imm8 imm) {
  rex_optional_r1(reg);
  write(0xB0 + (reg & DI), imm);
}

template<typename Imm>
void Assembler::movw(const Register reg, const Imm imm) {
  rex_optional_r2(reg);
  write(0xB8 + (reg & DI), imm);
}

template<typename Imm>
void Assembler::movd(const Register reg, const Imm imm) {
  rex_optional_r3(reg);
  write(0xB8 + (reg & DI), imm);
}

template<typename Imm>
void Assembler::movq(const Register reg, const Imm imm) {
  rex_optional_r4(reg);
  write(0xC7, EncodeModRM<0b11, 0>(reg), imm);
}

void Assembler::movb(const Register reg, const MemoryAddress &addr) {
  rex_optional_rm1(reg, addr);
  write(0x8A);
  write_address(addr, reg);
}

void Assembler::movw(const Register reg, const MemoryAddress &addr) {
  rex_optional_rm2(reg, addr);
  write(0x8B);
  write_address(addr, reg);
}

void Assembler::movd(const Register reg, const MemoryAddress &addr) {
  rex_optional_rm3(reg, addr);
  write(0x8B);
  write_address(addr, reg);
}

void Assembler::movq(const Register reg, const MemoryAddress &addr) {
  rex_optional_rm4(reg, addr);
  write(0x8B);
  write_address(addr, reg);
}

void Assembler::movb(const Register dest, const Register src) {
  rex_optional_rr1(dest, src);
  write(0x88, EncodeModRM<0b11>(src, dest));
}

void Assembler::movw(const Register dest, const Register src) {
  rex_optional_rr2(dest, src);
  write(0x89, EncodeModRM<0b11>(src, dest));
}

void Assembler::movd(const Register dest, const Register src) {
  rex_optional_rr3(dest, src);
  write(0x89, EncodeModRM<0b11>(src, dest));
}

void Assembler::movq(const Register dest, const Register src) {
  rex_optional_rr4(dest, src);
  write(0x89, EncodeModRM<0b11>(src, dest));
}

void Assembler::shlb(const Register dest, const Imm8 count) {
  rex_optional_r1(dest);
  write(0xC0, EncodeModRM<0b11, SP>(dest), count);
}

void Assembler::shlw(const Register dest, const Imm8 count) {
  rex_optional_r2(dest);
  write(0xC1, EncodeModRM<0b11, SP>(dest), count);
}

void Assembler::shld(const Register dest, const Imm8 count) {
  rex_optional_r3(dest);
  write(0xC1, EncodeModRM<0b11, SP>(dest), count);
}

void Assembler::shlq(const Register dest, const Imm8 count) {
  rex_optional_r4(dest);
  write(0xC1, EncodeModRM<0b11, SP>(dest), count);
}

void Assembler::shlb(const Register dest) {
  rex_optional_r1(dest);
  write(0xD0, EncodeModRM<0b11, SP>(dest));
}

void Assembler::shlw(const Register dest) {
  rex_optional_r2(dest);
  write(0xD1, EncodeModRM<0b11, SP>(dest));
}

void Assembler::shld(const Register dest) {
  rex_optional_r3(dest);
  write(0xD1, EncodeModRM<0b11, SP>(dest), 1);
}

void Assembler::shlq(const Register dest) {
  rex_optional_r4(dest);
  write(0xD1, EncodeModRM<0b11, SP>(dest));
}

void Assembler::shrb(const Register dest, Imm8 count) {
  rex_optional_r1(dest);
  write(0xC0, EncodeModRM<0b11, BX>(dest), count);
}

void Assembler::shrw(const Register dest, Imm8 count) {
  rex_optional_r2(dest);
  write(0xC1, EncodeModRM<0b11, BX>(dest), count);
}

void Assembler::shrd(const Register dest, Imm8 count) {
  rex_optional_r3(dest);
  write(0xC1, EncodeModRM<0b11, BX>(dest), count);
}

void Assembler::shrq(const Register dest, Imm8 count) {
  rex_optional_r4(dest);
  write(0x48, 0xC1, EncodeModRM<0b11, BX>(dest), count);
}

void Assembler::jmp(Imm8 rel8) {
  write(0xEB, rel8);
}

void Assembler::jmp(Imm16 rel16) {
  write(0xE9, rel16);
}

void Assembler::jmp(Imm32 rel32) {
  write(0xE9, rel32);
}

void Assembler::jmpw(const Register reg) {
  rex_optional_r2(reg);
  write(0xFF, EncodeModRM<0b11, SP>(reg));
}

void Assembler::jmpq(const Register reg) {
  rex_optional_r4(reg);
  write(0xFF, EncodeModRM<0b11, SP>(reg));
}

// NOTE: imul instruction order for rex is (src, dest), yes, this is
// correct.
void Assembler::imulw(const Register dest, const Register src) {
  rex_optional_rr2(src, dest);
  write(0x0F, 0xAF, EncodeModRM<0b11>(src, dest));
}

void Assembler::imuld(const Register dest, const Register src) {
  rex_optional_rr3(src, dest);
  write(0x0F, 0xAF, EncodeModRM<0b11>(src, dest));
}

void Assembler::imulq(const Register dest, const Register src) {
  rex_optional_rr4(src, dest);
  write(0x0F, 0xAF, EncodeModRM<0b11>(src, dest));
}

void Assembler::jump_cond(Condition cond, Imm8 rel8) {
  write(0x70 | cond, rel8);
}

void Assembler::jump_cond(Condition cond, Imm16 rel16) {
  write(0x80 | cond, rel16);
}

void Assembler::jump_cond(Condition cond, Imm32 rel32) {
  write(0x80 | cond, rel32);
}

void Assembler::lea(const Register reg,
                    const MemoryAddress &addr,
                    const OperandSize size) {
  switch (size) {
    case BYTE: break;
    case WORD:
      rex_optional_rm2(reg, addr);
      write(0x8D);
      write_address(addr, reg);
      break;
      case DWORD:
        rex_optional_rm3(reg, addr);
        write(0x8D);
        write_address(addr, reg);
        break;
        case QWORD:
          rex_optional_rm4(reg, addr);
          write(0x8D);
          write_address(addr, reg);
          break;
  }
}

void Assembler::movss(const Register dest, const Register src) {
  rex_optional_rr3(dest, src);
  write(0xF3, 0x0F, 0x10, EncodeModRM<0b11>(dest, src));
}

void Assembler::movss(const Register dest, const MemoryAddress &addr) {
  rex_optional_rm3(dest, addr);
  write(0xF3, 0x0F, 0x10);
  write_address(addr, dest);
}

void Assembler::movss(const MemoryAddress &dest, Register src) {
  rex_optional_rm3(src, dest);
  write(0xF3, 0x0F, 0x11);
  write_address(dest, src);
}

void Assembler::ret() {
  write(0xC3);
}