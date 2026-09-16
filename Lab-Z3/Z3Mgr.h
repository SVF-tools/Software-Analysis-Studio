//===- Z3Mgr.h -- Z3 manager for software verification ------------------===//
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

#ifndef SOFTWARE_ANALYSIS_STUDIO_LAB_Z3_Z3MGR_H
#define SOFTWARE_ANALYSIS_STUDIO_LAB_Z3_Z3MGR_H

#include "z3++.h"

namespace SVF {

#ifdef DEBUGINFO
#define DBOP(X) X;
#else
#define DBOP(X)
#endif

// An ObjVar's ID in SVFIR is marked using AddressMask (0x7f000000) to mimic a
// virtual memory address. This is not a physical runtime address. The flipped
// mask strips the address marker.
#define AddressMask 0x7f000000
#define FlippedAddressMask (AddressMask ^ 0xffffffff)

typedef unsigned u32_t;
typedef signed s32_t;

/// Z3 manager interface
class Z3Mgr {
public:
	/// Constructor
	Z3Mgr(u32_t numOfMapElems)
	: solver(ctx)
	, varID2ExprMap(ctx)
	, lastSlot(numOfMapElems) {
		resetZ3ExprMap();
	}

	/// Reset and reinitialize Z3 expressions.
	/// varID2ExprMap maps a variable ID to its Z3 expression.
	/// loc2ValMap maps an address location to its stored value.
	inline void resetZ3ExprMap() {
		varID2ExprMap.resize(lastSlot + 1);
		z3::expr loc2ValMap = ctx.constant("loc2ValMap", ctx.array_sort(ctx.int_sort(), ctx.int_sort()));
		updateZ3Expr(lastSlot, loc2ValMap);
	}

	/// Store and select for loc2ValMap, i.e., store and load
	z3::expr storeValue(const z3::expr loc, const z3::expr value);
	z3::expr loadValue(const z3::expr loc);

	/// The virtual address starts with 0x7f...... + idx
	inline u32_t getVirtualMemAddress(u32_t idx) const {
		return AddressMask + idx;
	}

	inline bool isVirtualMemAddress(u32_t val) {
		return (val > 0 && (val & AddressMask) == AddressMask);
	}

	inline bool isVirtualMemAddress(z3::expr e) {
		z3::expr val = getEvalExpr(e);
		if (val.is_numeral())
			return isVirtualMemAddress(z3Expr2NumValue(val));
		return false;
	}

	/// Return the internal index if idx is an address; otherwise return idx
	inline u32_t getInternalID(u32_t idx) const {
		return (idx & FlippedAddressMask);
	}

	/// Return a Z3 expression based on a variable ID
	inline z3::expr getZ3Expr(u32_t idx) const {
		assert(getInternalID(idx) == idx && "variable idx overflow > 0x7f000000?");
		assert(varID2ExprMap.size() >= idx + 1 && "idx out of bounds; increase map size!");
		return varID2ExprMap[getInternalID(idx)];
	}

	/// Update an expression when processing an assignment
	inline void updateZ3Expr(u32_t idx, z3::expr target) {
		assert(varID2ExprMap.size() >= idx + 1 && "idx out of bounds; increase map size!");
		varID2ExprMap.set(getInternalID(idx), target);
	}

	/// Return an integer value from an expression if it is a numeral
	s32_t z3Expr2NumValue(z3::expr e);

	/// Evaluate an expression using a model satisfying the current constraints
	z3::expr getEvalExpr(z3::expr e);

	/// Print all expression values after evaluation
	void printExprValues();

	/// Print all Z3 expressions
	void printZ3Exprs();

	/// Return the Z3 solver
	inline z3::solver& getSolver() {
		return solver;
	}

	/// Return the Z3 context
	inline z3::context& getCtx() {
		return ctx;
	}

	/// Clean up the expression map
	inline void clearVarID2ExprMap() {
		while (!varID2ExprMap.empty())
			varID2ExprMap.pop_back();
		resetZ3ExprMap();
	}

	/// Add not(Q) to the solver. Return true when no counterexample exists.
	bool checkNegateAssert(z3::expr q) {
		getSolver().push();
		getSolver().add(!q);
		bool result = getSolver().check() == z3::unsat;
		getSolver().pop();
		return result;
	}

protected:
	z3::context ctx;
	z3::solver solver;

private:
	z3::expr_vector varID2ExprMap;
	u32_t lastSlot;
};

} // namespace SVF

#endif // SOFTWARE_ANALYSIS_STUDIO_LAB_Z3_Z3MGR_H
