#include <iostream>
#include <cstdlib>
#define SIZE 100000 //WE WILL CHANGE THIS SOON
using namespace std;
int main() {
 cout << "BEFORE MALLOC" << endl;
 malloc(SIZE);
 cout << "AFTER MALLOC" << endl;
 return 0;
}
