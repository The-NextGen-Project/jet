//
// Created by Owner on 9/16/2021.
//

#ifndef JET_X64_ASSEMBLER_H
#define JET_X64_ASSEMBLER_H

#include "util/misc.h"

typedef uint8_t  ubyte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;

#define instruction_unary(name) \
void name##b(Register reg); \
void name##w(Register reg); \
void name##d(Register reg); \
void name##q(Register reg);     \
\
void name##b(const MemoryAddress &addr); \
void name##w(const MemoryAddress &addr); \
void name##d(const MemoryAddress &addr); \
void name##q(const MemoryAddress &addr); \

#define instruction_binary(name) \
void name##b(Register dest, Register src);\
void name##w(Register dest, Register src);\
void name##d(Register dest, Register src);\
void name##q(Register dest, Register src);\
\
void name##b(Register dest, const MemoryAddress &src);\
void name##w(Register dest, const MemoryAddress &src);\
void name##d(Register dest, const MemoryAddress &src);\
void name##q(Register dest, const MemoryAddress &src);\
\
void name##b(const MemoryAddress &dest, Register src);\
void name##w(const MemoryAddress &dest, Register src);\
void name##d(const MemoryAddress &dest, Register src);\
void name##q(const MemoryAddress &dest, Register src);\
\
void name##b(Register reg, Imm8 imm); \
void name##w(Register reg, Imm8 imm); \
void name##d(Register reg, Imm8 imm); \
void name##q(Register reg, Imm8 imm); \
\
void name##w(Register reg, Imm16 imm);\
void name##w(Register reg, Imm32 imm);\
\
void name##d(Register reg, Imm16 imm);\
void name##d(Register reg, Imm32 imm);\
\
void name##q(Register reg, Imm16 imm);\
void name##q(Register reg, Imm32 imm);    \

#define instruction_float_binary(name) \
void name##ss(Register dest, Register src); \
void name##ss(Register reg, const MemoryAddress &addr);\
\
void name##sd(Register dest, Register src);\
void name##sd(Register reg, const MemoryAddress &addr);\

namespace nextgen { namespace jet { namespace x64 {

typedef struct Imm8 { ubyte value; }    Imm8;
typedef struct Imm16 { ushort value; }  Imm16;
typedef struct Imm32 { uint value; }    Imm32;
typedef struct Imm64 { ulong value; }   Imm64;

enum Condition {
  Overflow = 0,
  NotOverflow = 1,
  Below       = 2,
  AboveOrEqual = 3,
  Equal     = 4,
  NotEqual  = 5,
  BelowOrEqual = 6,
  Above     = 7,
  Signed    = 8,
  NotSigned = 9,
  LessThan = 0xC,
  LessThanEqual = 0xE,
  GreaterThan   = 0xF,
  GreaterThanEqual = 0xD,
};

enum REX {
  REX_PREFIX = 0b01000000,
  REX_W = 0b00001000,
  REX_R = 0b00000100,
  REX_X = 0b00000010,
  REX_B = 0b00000001
};

enum MOD {
  MOD_INDIRECT = 0b00,
  MOD_DISP8    = 0b01,
  MOD_DISP32   = 0b10,
  MOD_REG      = 0b11
};


enum Register : ubyte {
  AX = 0b000, CX = 0b001, DX = 0b010, BX = 0b011,
  SP = 0b100, BP = 0b101, SI = 0b110, DI = 0b111,

  R8, R9, R10, R11, R12, R13, R14, R15,

  XMM0 = 0b000, XMM1 = 0b001, XMM2 = 0b010, XMM3 = 0b011,
  XMM4 = 0b100, XMM5 = 0b101, XMM6 = 0b110, XMM7 = 0b111,

  XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15
};


enum OperandSize {
  BYTE,
  WORD,
  DWORD,
  QWORD,
  };

// Reference: https://blog.yossarian.net/2020/06/13/How-x86_64-addresses-memory
enum MemoryAddressKind {
  Disp32,               // [disp32]
  Base,                 // [base]
  BaseIndex,            // [base + index]
  BaseDisp32,           // [base + disp32]
  BaseDisp8,            // [base + disp8]
  BaseIndexScale,       // [base + (index * scale)]
  BaseIndexDisp32,      // [base + (index * scale) + disp32]
  BaseIndexDisp8,       // [base + (index * scale) + disp8]
};

enum SibScale : ubyte {
  Scale1 = 0b00,
  Scale2 = 0b01,
  Scale4 = 0b10,
  Scale8 = 0b11
};

struct MemoryAddress {
  bool has_disp8  = false;
  bool has_disp32 = false;
  bool has_sib    = false;

  ubyte    scale = Scale1;
  ubyte    mod   = MOD_INDIRECT;
  uint     disp  = 0;

  Register base  = BP;
  Register index = SP;

  // [disp32]
  explicit MemoryAddress(uint disp32);

  // [base]
  explicit MemoryAddress(Register base);

  // [base + disp8]
  explicit MemoryAddress(Register base, ubyte disp8);

  // [base + disp32]
  explicit MemoryAddress(Register base, uint disp32);

  // [base + (index * scale)]
  explicit MemoryAddress(Register base,
                         Register index,
                         SibScale scale);

  // [base + (index * scale) + disp8]
  explicit MemoryAddress(Register base, Register index, SibScale,
                         ubyte disp8);

  // [base + (index * scale) + disp32]
  explicit MemoryAddress(Register base, Register index, SibScale,
                         uint disp32);
};

class Assembler {
private:
  ubyte *memory;
  size_t used = 0;

  // Writing primitive values to buffer
  void write(ubyte value);
  void write_16(ushort value);
  void write_32(uint value);
  void write_64(ulong value);

  // Write Immediate values to the buffer
  void write(Imm8 imm);
  void write(Imm16 imm);
  void write(Imm32 imm);
  void write(Imm64 imm);

  // Write a memory address given the register destination
  /// Reference: http://www.c-jump.com/CIS77/CPU/x86/lecture.html
  void write_address(const MemoryAddress &addr, Register dest);


  // Some useful overrides when writing multiple values.
  // There is no need for a custom implementation because
  // we don't write more than 4 values at a given time in a row.
  //
  // We depend on the compiler on choosing the correct function for the
  // value that is passed in -- the choice should be clear given distinct
  // parameter values.

  template<typename T, typename K>
  void write(T f, K s);

  template<typename T, typename K, typename L>
  void write(T f, K s, L t);

  template<typename T, typename K, typename L, typename Q>
  void write(T f, K s, L t, Q fr);

  /// Write rex-byte based on a single register for a BYTE operation.
  void rex_optional_r1(Register reg);

  /// Write rex-byte based on a single register for a WORD operation.
  void rex_optional_r2(Register reg);

  /// Write rex-byte based on a single register for a DWORD operation.
  void rex_optional_r3(Register reg);

  /// Writes rex-values for 64-bit argument operations. This applies for
  /// the long-mode of x86-- which is x64. It also integrates rex-prefix
  /// with special register R8-R15.
  void rex_optional_r4(Register reg);

  /// Binary Operation Rex Byte -- BYTE SIZE
  void rex_optional_rr1(Register dest, Register src);

  /// Binary Operation Rex Byte -- WORD SIZE
  void rex_optional_rr2(Register dest, Register src);


  /// Binary Operation Rex Byte -- DWORD SIZE
  void rex_optional_rr3(Register dest, Register src);

  /// Binary Operation Rex Byte -- QWORD SIZE
  void rex_optional_rr4(Register dest, Register src);

  /// Memory Operation Rex Byte -- BYTE SIZE
  void rex_optional_rm1(Register reg, const MemoryAddress &addr);

  /// Memory Operation Rex Byte -- WORD SIZE
  void rex_optional_rm2(Register reg, const MemoryAddress &addr);

  /// Memory Operation Rex Byte -- DWORD SIZE
  void rex_optional_rm3(Register reg, const MemoryAddress &addr);

  /// Memory Operation Rex Byte -- QWORD SIZE
  void rex_optional_rm4(Register reg, const MemoryAddress &addr);


public:

  explicit Assembler(size_t size);
  explicit Assembler(size_t size, bool executable);

  /// Spill the memory buffer containing machine code bytes written
  ubyte *spill() const;

  /// The number of bytes that have been written to the memory buffer
  ulong bytes() const;

  /// Get a given byte that has been generated by the assembler
  ubyte operator[](size_t index) { return memory[index]; }

  void popq(Register reg);
  void popw(Register reg);

  void pushq(Register reg);
  void pushw(Register reg);

  void movb(Register reg, Imm8 imm);
  template<typename Imm>
  void movw(Register reg, Imm imm);
  template<typename Imm>
  void movd(Register reg, Imm imm);
  template<typename Imm>
  void movq(Register reg, Imm imm);

  void movb(Register reg, const MemoryAddress &addr);
  void movw(Register reg, const MemoryAddress &addr);
  void movd(Register reg, const MemoryAddress &addr);
  void movq(Register reg, const MemoryAddress &addr);

  void movb(Register dest, Register src);
  void movw(Register dest, Register src);
  void movd(Register dest, Register src);
  void movq(Register dest, Register src);

  void shlb(Register dest, Imm8 count);
  void shlw(Register dest, Imm8 count);
  void shld(Register dest, Imm8 count);
  void shlq(Register dest, Imm8 count);

  void shlb(Register dest);
  void shlw(Register dest);
  void shld(Register dest);
  void shlq(Register dest);

  void shrb(Register dest, Imm8 count);
  void shrw(Register dest, Imm8 count);
  void shrd(Register dest, Imm8 count);
  void shrq(Register dest, Imm8 count);

  void jmp(Imm8 rel8);
  void jmp(Imm16 rel16);
  void jmp(Imm32 rel32);

  void jmpw(Register reg);
  void jmpq(Register reg);

  void jump_cond(Condition cond, Imm8 rel8);
  void jump_cond(Condition cond, Imm16 rel16);
  void jump_cond(Condition cond, Imm32 rel32);

  // NOTE: imul instruction order for rex is (src, dest), yes, this is
  // correct.
  void imulw(Register dest, Register src);
  void imuld(Register dest, Register src);
  void imulq(Register dest, Register src);

  void lea(Register reg, const MemoryAddress &addr, OperandSize size);

  void movss(Register dest, Register src);
  void movss(Register dest, const MemoryAddress &addr);
  void movss(const MemoryAddress& dest, Register src);

  instruction_binary(add);
  instruction_binary(sub);
  instruction_binary(and);
  instruction_binary(cmp);
  instruction_binary(xor);
  instruction_binary(or);

  instruction_float_binary(add);
  instruction_float_binary(sub);
  instruction_float_binary(div);
  instruction_float_binary(mul);

  instruction_unary(inc);
  instruction_unary(dec);
  instruction_unary(not);
  instruction_unary(mul);
  instruction_unary(neg);
  instruction_unary(div);
  instruction_unary(idiv);

  void ret();
};
}}}

#endif //JET_X64_ASSEMBLER_H


