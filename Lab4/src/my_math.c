// simple pow implementation
double pow(double x, double y) {
    return x * pow(x, y - 1);
}