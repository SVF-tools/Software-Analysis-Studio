//===- test.cpp -- Manual assertion-based verification driver ------------===//
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
 * Manual symbolic execution for assertion-based verification
 *
 * Created on: Feb 19, 2024
 */

#include "Util/SVFUtil.h"
#include "Z3Examples.h"

using namespace SVF;
using namespace SVFUtil;

/*
 * Lab-Z3 main function entry.
 * To run test0, use `bin/lab-z3 test0`.
 * To debug it, set "program" to "${workspaceFolder}/bin/lab-z3" and
 * "args" to ["test0"] in .vscode/launch.json.
 */
int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: ./lab-z3 test0" << std::endl;
		return 1;
	}

	Z3Examples z3Mgr(1000);
	bool result = false;
	std::string testName = argv[1];

	if (testName == "test0") {
		z3Mgr.test0();
		// assert(x == 5);
		z3::expr assertCond = z3Mgr.getZ3Expr("x") == z3Mgr.getZ3Expr(5);
		result = z3Mgr.checkNegateAssert(assertCond);
	}
	else if (testName == "test1") {
		z3Mgr.test1();
		// assert(b > 0);
		// TODO: Write the assertion check
	}
	else if (testName == "test2") {
		z3Mgr.test2();
		// assert(b > 3);
		// TODO: Write the assertion check
	}
	else if (testName == "test3") {
		z3Mgr.test3();
		// assert(x == 10);
		// TODO: Write the assertion check
	}
	else if (testName == "test4") {
		z3Mgr.test4();
		// assert((a + b) > 20);
		// TODO: Write the assertion check
	}
	else if (testName == "test5") {
		z3Mgr.test5();
		// assert(b1 >= 5);
		// TODO: Write the assertion check
	}
	else if (testName == "test6") {
		z3Mgr.test6();
		// assert(*p == 5);
		// TODO: Write the assertion check
	}
	else if (testName == "test7") {
		z3Mgr.test7();
		// assert(d == 5);
		// TODO: Write the assertion check
	}
	else if (testName == "test8") {
		z3Mgr.test8();
		// assert(*p == 0);
		// TODO: Write the assertion check
	}
	else if (testName == "test9") {
		z3Mgr.test9();
		// assert(z == 15);
		// TODO: Write the assertion check
	}
	else if (testName == "test10") {
		z3Mgr.test10();
		// assert(x == 3 && y == 2);
		// TODO: Write the assertion check
	}
	else {
		std::cerr << "Invalid test name: " << testName << std::endl;
		return 1;
	}

	if (result) {
		std::cout << testName << " passed!!" << std::endl;
	}
	else {
		std::cout << SVFUtil::errMsg(testName)
		          << SVFUtil::errMsg(" assertion check failed!!") << std::endl;
		assert(result);
	}

	z3Mgr.resetSolver();
	return 0;
}
