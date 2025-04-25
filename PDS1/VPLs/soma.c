#include <iostream>


void sum(int a, int b) {
    cout << "Sum of int: " << (a + b) << endl;
}
void sum(double a, double b) {
    cout << "Sum of double: " << (a + b) << endl;
}

int main() {
    sum(1, 2);
    sum(1.1, 2.2);
    sum(1, 2.2);
    sum((int) 1.1, (int) 2.2);
    return 0;
} 