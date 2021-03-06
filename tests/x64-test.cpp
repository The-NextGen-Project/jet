#ifndef TEST
  #define TEST(suite, name) void suite##_##name##_##test()
#endif
#ifndef TEST_SUITE_MAIN
  #define TEST_SUITE_MAIN(suite) void main_##suite()
#endif
#ifndef TEST_CALL
  #define TEST_CALL(suite, name) suite##_##name##_##test()
#endif

#include <nextgen/jet/jet-x64-assembler.h>
#include <nextgen/panic.h>

TEST(x64Test, Add) {

  using namespace nextgen::jet::x64;
  using namespace nextgen;

  Assembler assembler = Assembler(1000);
  for (Register r = Register::AX; r <= Register::R15; r = Register(ubyte(r) +1)) {
    assembler.addb((Register)r, (Register)r);
    assembler.addw((Register)r, (Register)r);
    assembler.addd((Register)r, (Register)r);
    assembler.addq((Register)r, (Register)r);
  }

  auto number_of_bytes = assembler.bytes();
  auto bytes = assembler.spill();

  ubyte correct_output[] = { 0x00, 0xC0, 0x66, 0x01, 0xC0, 0x01, 0xC0, 0x48,
                             0x01, 0xC0, 0x00, 0xC9, 0x66, 0x01, 0xC9, 0x01,
                             0xC9, 0x48, 0x01, 0xC9, 0x00, 0xD2, 0x66, 0x01,
                             0xD2, 0x01, 0xD2, 0x48, 0x01, 0xD2, 0x00, 0xDB,
                             0x66, 0x01, 0xDB, 0x01, 0xDB, 0x48, 0x01, 0xDB,
                             0x40, 0x00, 0xE4, 0x66, 0x01, 0xE4, 0x01, 0xE4,
                             0x48, 0x01, 0xE4, 0x40, 0x00, 0xED, 0x66, 0x01,
                             0xED, 0x01, 0xED, 0x48, 0x01, 0xED, 0x40, 0x00,
                             0xF6, 0x66, 0x01, 0xF6, 0x01, 0xF6, 0x48, 0x01,
                             0xF6, 0x40, 0x00, 0xFF, 0x66, 0x01, 0xFF, 0x01,
                             0xFF, 0x48, 0x01, 0xFF, 0x45, 0x00, 0xC0, 0x66,
                             0x45, 0x01, 0xC0, 0x45, 0x01, 0xC0, 0x4D, 0x01,
                             0xC0, 0x45, 0x00, 0xC9, 0x66, 0x45, 0x01, 0xC9,
                             0x45, 0x01, 0xC9, 0x4D, 0x01, 0xC9, 0x45, 0x00,
                             0xD2, 0x66, 0x45, 0x01, 0xD2, 0x45, 0x01, 0xD2,
                             0x4D, 0x01, 0xD2, 0x45, 0x00, 0xDB, 0x66, 0x45,
                             0x01, 0xDB, 0x45, 0x01, 0xDB, 0x4D, 0x01, 0xDB,
                             0x45, 0x00, 0xE4, 0x66, 0x45, 0x01, 0xE4, 0x45,
                             0x01, 0xE4, 0x4D, 0x01, 0xE4, 0x45, 0x00, 0xED,
                             0x66, 0x45, 0x01, 0xED, 0x45, 0x01, 0xED, 0x4D,
                             0x01, 0xED, 0x45, 0x00, 0xF6, 0x66, 0x45, 0x01,
                             0xF6, 0x45, 0x01, 0xF6, 0x4D, 0x01, 0xF6, 0x45,
                             0x00, 0xFF, 0x66, 0x45, 0x01, 0xFF, 0x45, 0x01,
                             0xFF, 0x4D, 0x01, 0xFF } ;

  FOR(i, number_of_bytes) {
    ASSERT_EQ(bytes[i], correct_output[i]);
//    Console::Log(std::hex, std::setfill('0'), std::setw(2), (ushort)bytes[i], ""
//                                                                            " ");
  }
}

TEST(x64Test, Sub) {
  using namespace nextgen::jet::x64;
  using namespace nextgen;

  Assembler assembler = Assembler(1000);
  for (Register r = Register::AX; r <= Register::R15; r = Register(ubyte(r) +1)) {
    assembler.subb((Register)r, (Register)r);
    assembler.subw((Register)r, (Register)r);
    assembler.subd((Register)r, (Register)r);
    assembler.subq((Register)r, (Register)r);
  }

  auto number_of_bytes = assembler.bytes();
  auto bytes = assembler.spill();

  ubyte correct_output[] = {  0x28, 0xC0, 0x66, 0x29, 0xC0, 0x29, 0xC0, 0x48, 0x29,
                               0xC0, 0x28, 0xC9, 0x66, 0x29, 0xC9, 0x29, 0xC9, 0x48,
                               0x29, 0xC9, 0x28, 0xD2, 0x66, 0x29, 0xD2, 0x29, 0xD2,
                               0x48, 0x29, 0xD2, 0x28, 0xDB, 0x66, 0x29, 0xDB, 0x29,
                               0xDB, 0x48, 0x29, 0xDB, 0x40, 0x28, 0xE4, 0x66, 0x29,
                               0xE4, 0x29, 0xE4, 0x48, 0x29, 0xE4, 0x40, 0x28, 0xED,
                               0x66, 0x29, 0xED, 0x29, 0xED, 0x48, 0x29, 0xED, 0x40,
                               0x28, 0xF6, 0x66, 0x29, 0xF6, 0x29, 0xF6, 0x48, 0x29,
                               0xF6,
                               0x40, 0x28, 0xFF, 0x66, 0x29, 0xFF, 0x29, 0xFF, 0x48,
                               0x29, 0xFF, 0x45, 0x28, 0xC0, 0x66, 0x45, 0x29, 0xC0,
                               0x45, 0x29, 0xC0, 0x4D, 0x29, 0xC0, 0x45, 0x28, 0xC9,
                               0x66, 0x45, 0x29, 0xC9, 0x45, 0x29, 0xC9, 0x4D, 0x29,
                               0xC9, 0x45, 0x28, 0xD2, 0x66, 0x45, 0x29, 0xD2, 0x45, 0x29, 0xD2, 0x4D, 0x29, 0xD2, 0x45,
                               0x28, 0xDB, 0x66, 0x45, 0x29, 0xDB, 0x45, 0x29,
                               0xDB, 0x4D, 0x29, 0xDB, 0x45, 0x28, 0xE4, 0x66, 0x45, 0x29, 0xE4, 0x45, 0x29, 0xE4, 0x4D,
                               0x29, 0xE4, 0x45, 0x28, 0xED, 0x66, 0x45, 0x29, 0xED, 0x45, 0x29, 0xED,
                               0x4D, 0x29, 0xED, 0x45, 0x28, 0xF6, 0x66, 0x45, 0x29,
                               0xF6, 0x45, 0x29, 0xF6, 0x4D, 0x29, 0xF6, 0x45, 0x28,
                               0xFF, 0x66, 0x45, 0x29, 0xFF, 0x45, 0x29, 0xFF, 0x4D, 0x29, 0xFF };

  FOR(i, number_of_bytes) {
    ASSERT_EQ(bytes[i], correct_output[i]);
//        Console::Log(std::hex, std::setfill('0'), std::setw(2), (ushort)bytes[i], ""
//                                                                                " ");
  }
}

TEST_SUITE_MAIN(x64Test) {
  TEST_CALL(x64Test, Add);
  TEST_CALL(x64Test, Sub);
}

