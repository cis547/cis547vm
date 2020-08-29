int main() {
  int a, b;
  int c = 0;
  b = c;
  a = b;
  int d = a / c; // Divide by zero
  c = c + 1;
  c = c * 0;
  int e = a / c;
  return 0;
}
