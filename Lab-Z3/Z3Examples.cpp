//===- Z3Examples.cpp -- Manual assertion-based verification ------------===//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * Manual assertion-based verification with Z3
 *
 * Created on: Feb 19, 2024
 */

#include "Z3Examples.h"

using namespace SVF;
using namespace z3;

/*
int main() {
    int* p;
    int q;
    int* r;
    int x;

    p = malloc();
    q = 5;
    *p = q;
    x = *p;
    assert(x == 5);
}
*/
void Z3Examples::test0() {
	// int* p;
	expr p = getZ3Expr("p");

	// int q;
	expr q = getZ3Expr("q");

	// int* r;
	expr r = getZ3Expr("r");

	// int x;
	expr x = getZ3Expr("x");

	// p = malloc();
	addToSolver(p == getMemObjAddress("malloc"));

	// q = 5;
	addToSolver(q == 5);

	// *p = q;
	storeValue(p, q);

	// x = *p;
	addToSolver(x == loadValue(p));
}

/*
int main() {
    int a;
    int b;
    a = 0;
    b = a + 1;
    assert(b > 0);
}
*/
void Z3Examples::test1() {
	// int a;
	expr a = getZ3Expr("a");
	// int b;
	expr b = getZ3Expr("b");
	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int* p;
    int q;
    int b;
    p = malloc();
    *p = 0;
    q = *p;
    *p = 3;
    b = *p + 1;
    assert(b > 3);
}
*/
void Z3Examples::test2() {
	// int* p;
	expr p = getZ3Expr("p");
	// int q;
	expr q = getZ3Expr("q");
	// int b;
	expr b = getZ3Expr("b");
	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int** p;
    int* q;
    int* r;
    int x;

    p = malloc1();
    q = malloc2();
    *p = q;
    *q = 10;
    r = *p;
    x = *r;
    assert(x == 10);
}
*/
void Z3Examples::test3() {
	// int** p;
	expr p = getZ3Expr("p");

	// int* q;
	expr q = getZ3Expr("q");

	// int* r;
	expr r = getZ3Expr("r");

	// int x;
	expr x = getZ3Expr("x");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int* p;
    int* x;
    int* y;
    int a;
    int b;
    p = malloc();
    x = &p[0];
    y = &p[1];
    *x = 10;
    *y = 11;
    a = *x;
    b = *y;
    assert((a + b) > 20);
}
*/
void Z3Examples::test4() {
	// int* p;
	expr p = getZ3Expr("p");

	// int* x;
	expr x = getZ3Expr("x");

	// int* y;
	expr y = getZ3Expr("y");

	// int a;
	expr a = getZ3Expr("a");

	// int b;
	expr b = getZ3Expr("b");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int main(int argv) {
    int a;
    int b;
    int b1;
    a = argv + 1;
    b = 5;
    if (a > 10)
        b = a;
    b1 = b;
    assert(b1 >= 5);
}
*/
void Z3Examples::test5() {
	// int argv;
	expr argv = getZ3Expr("argv");

	// int a;
	expr a = getZ3Expr("a");

	// int b;
	expr b = getZ3Expr("b");

	// int b1;
	expr b1 = getZ3Expr("b1");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int* a = malloc1();
    int* b = malloc2();
    *a = 5;
    *b = 10;
    int* p;
    if (*a < *b)
        p = a;
    else
        p = b;
    assert(*p == 5);
}
*/
void Z3Examples::test6() {
	// int* a;
	expr a = getZ3Expr("a");
	// int* b;
	expr b = getZ3Expr("b");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int a = 1, b = 2, c = 3;
    int d;
    if (a > 0)
        d = b + c;
    else
        d = b - c;
    assert(d == 5);
}
*/
void Z3Examples::test7() {
	// int a = 1, b = 2, c = 3;
	expr a = getZ3Expr("a");
	expr b = getZ3Expr("b");
	expr c = getZ3Expr("c");
	// int d;
	expr d = getZ3Expr("d");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int main() {
    int arr[2] = {0, 1};
    int a = 10;
    int* p;
    if (a > 5)
        p = &arr[0];
    else
        p = &arr[1];
    assert(*p == 0);
}
*/
void Z3Examples::test8() {
	// int arr[2];
	expr arr = getZ3Expr("arr");
	// int* p;
	expr p = getZ3Expr("p");

	/// TODO: Write the Z3 expressions for the program above
}

/*
struct A {
    int f0;
    int* f1;
};

int main() {
    struct A* p;
    int* x;
    int* q;
    int** r;
    int* y;
    int z;

    p = malloc1();
    x = malloc2();
    *x = 5;
    q = &(p->f0);
    *q = 10;
    r = &(p->f1);
    *r = x;
    y = *r;
    z = *q + *y;
    assert(z == 15);
}
*/
void Z3Examples::test9() {
	// struct A* p;
	expr p = getZ3Expr("p");

	// int* x;
	expr x = getZ3Expr("x");

	// int* q;
	expr q = getZ3Expr("q");

	// int** r;
	expr r = getZ3Expr("r");

	// int* y;
	expr y = getZ3Expr("y");

	// int z;
	expr z = getZ3Expr("z");

	/// TODO: Write the Z3 expressions for the program above
}

/*
int foo(int z) {
    int k = z;
    return k;
}

int main() {
    int x;
    int y;
    y = foo(2);
    x = foo(3);
    assert(x == 3 && y == 2);
}
*/
void Z3Examples::test10() {
	// int x;
	expr x = getZ3Expr("x");

	// int y;
	expr y = getZ3Expr("y");

	// int z;
	expr z = getZ3Expr("z");

	// int k;
	expr k = getZ3Expr("k");

	/// TODO: Write the Z3 expressions for the program above
}
