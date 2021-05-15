int main() {
  for (int i = 0; i < 5; i++) {
    asm volatile("svc 0");
  }

  return 0;
}