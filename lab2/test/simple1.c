int main(int argc, char **argv) {
  if (argc > 2) {
    int x = 0;
    int y = x;
    int z = y / x; // Divide by zero
  }
  return 0;
}
