#define V(suite) extern void main_##suite();
#define TEST(suite) main_##suite()

#define DEF_ALL_TESTS \
  V(LexTest)          \
  V(ParseTest)        \
  V(x64Test)          \

DEF_ALL_TESTS
int main(int argc, char **argv) {
  TEST(LexTest);
  TEST(ParseTest);
  TEST(x64Test);
}
