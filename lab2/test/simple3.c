int main() {
  int a = 0;
  int b = 0;
  int d = 5;
  int c = a == b;
  int e = d / (c * 0); // divide by zero
  return 0;
}
