//===- Z3Mgr.cpp -- Z3 manager for software verification ----------------===//
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
 * Z3 manager for software verification
 *
 * Created on: Feb 19, 2024
 */

#include "Z3Mgr.h"
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace SVF;
using namespace z3;

/// Store and select for loc2ValMap, i.e., store and load.
/// The address is evaluated before loc2ValMap is accessed.
z3::expr Z3Mgr::storeValue(const z3::expr loc, const z3::expr value) {
	z3::expr addr = getEvalExpr(loc);
	assert(isVirtualMemAddress(addr) && "pointer operand is not a virtual address?");
	z3::expr loc2ValMap = varID2ExprMap[lastSlot];
	loc2ValMap = z3::store(loc2ValMap, addr, value);
	varID2ExprMap.set(lastSlot, loc2ValMap);
	return loc2ValMap;
}

z3::expr Z3Mgr::loadValue(const z3::expr loc) {
	z3::expr addr = getEvalExpr(loc);
	assert(isVirtualMemAddress(addr) && "pointer operand is not a virtual address?");
	z3::expr loc2ValMap = varID2ExprMap[lastSlot];
	return z3::select(loc2ValMap, addr);
}

/// Return an integer value from an expression if it is a numeral
s32_t Z3Mgr::z3Expr2NumValue(z3::expr e) {
	z3::expr val = getEvalExpr(e);
	if (val.is_numeral())
		return val.get_numeral_int64();
	assert(false && "expression is not a numeral");
	abort();
}

/// Evaluate an expression using a model satisfying the current constraints
z3::expr Z3Mgr::getEvalExpr(z3::expr e) {
	z3::check_result result = solver.check();
	assert(result != z3::unsat && "constraints are unsatisfiable");
	z3::model model = solver.get_model();
	return model.eval(e);
}

/// Print all expression values after evaluation
void Z3Mgr::printExprValues() {
	std::cout.flags(std::ios::left);
	std::cout << "-----------Var and Value-----------\n";
	for (u32_t i = 0; i < lastSlot; ++i) {
		expr e = getEvalExpr(varID2ExprMap[i]);
		if (e.is_numeral()) {
			s32_t value = e.get_numeral_int64();
			std::stringstream exprName;
			exprName << "Var" << i;
			std::cout << std::setw(25) << exprName.str();
			if (isVirtualMemAddress(value))
				std::cout << "\t Value: " << std::hex << "0x" << value << "\n";
			else
				std::cout << "\t Value: " << std::dec << value << "\n";
		}
	}
	std::cout << "-----------------------------------------\n";
}

void Z3Mgr::printZ3Exprs() {
	std::cout << solver << "\n";
}
