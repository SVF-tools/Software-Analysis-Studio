//===- Z3Mgr.h -- Z3 manager for software verification ------------------//
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

#ifndef SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H
#define SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H

#include "z3++.h"

namespace SVF {

#ifdef DEBUGINFO
#	define DBOP(X) X;
#else
#	define DBOP(X)
#endif

// an ObjVar's ID in PAG will be marked using AddressMask (0x7f000000) to mimic the virtual memory address.
// Note that this is not a physical runtime address but an abstract address to easy debugging purposes
// FlippedAddressMask is used to strip off the mask
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

		/// reset and reinitialize Z3Exprs. 
		/// varID2ExprMap: maps a variable id to its z3 expression
		/// loc2ValMap: maps an address location to its stored value, e.g., loc2ValMap[addr] = val
		inline void resetZ3ExprMap() {
			varID2ExprMap.resize(lastSlot + 1);
			z3::expr loc2ValMap = ctx.constant("loc2ValMap", ctx.array_sort(ctx.int_sort(), ctx.int_sort()));
			updateZ3Expr(lastSlot, loc2ValMap);
		}

		/// Store and Select for Loc2ValMap, i.e., store and load
		z3::expr storeValue(const z3::expr loc, const z3::expr value);
		z3::expr loadValue(const z3::expr loc);

		/// The physical address starts with 0x7f...... + idx
		inline u32_t getVirtualMemAddress(u32_t idx) const {
			return AddressMask + idx;
		}

		inline bool isVirtualMemAddress(u32_t val) {
			return (val > 0 && (val & AddressMask) == AddressMask);
			// return ((val & AddressMask) > 0);
		}

		inline bool isVirtualMemAddress(z3::expr e) {
			z3::expr val = getEvalExpr(e);
			if (val.is_numeral()) {
				return isVirtualMemAddress(z3Expr2NumValue(val));
			}
			else {
				return false;
			}
		}

		/// Return the internal index if idx is an address otherwise return the value of idx
		inline u32_t getInternalID(u32_t idx) const {
			return (idx & FlippedAddressMask);
		}

		/// Return Z3 expression based on SVFVar ID
		inline z3::expr getZ3Expr(u32_t idx) const {
			assert(getInternalID(idx) == idx && "SVFVar idx overflow > 0x7f000000?");
			assert(varID2ExprMap.size() >= idx + 1 && "idx out of bound for map access, increase map size!");
			return varID2ExprMap[getInternalID(idx)];
		}

		/// Update expression when assignments
		inline void updateZ3Expr(u32_t idx, z3::expr target) {
			assert(varID2ExprMap.size() >= idx + 1 && "idx out of bound for map access, increase map size!");
			varID2ExprMap.set(getInternalID(idx), target);
		}

		/// Return int value from an expression if it is a numeral, otherwise return an approximate value
		s32_t z3Expr2NumValue(z3::expr e);

		/// It checks if the constraints added to the Z3 solver are satisfiable.
		/// If they are, it retrieves the model that satisfies these constraints
		/// and evaluates the given complex expression e within this model, returning the evaluated result
		z3::expr getEvalExpr(z3::expr e);

		/// Print all expressions' values after evaluation
		void printExprValues();

		// Print all Z3 expressions
		void printZ3Exprs();

		/// Return the z3 solver
		inline z3::solver& getSolver() {
			return solver;
		}
		/// Return the z3 solver context (typically corresponding to a program)
		inline z3::context& getCtx() {
			return ctx;
		}

		// Clean up the maps
		inline void clearVarID2ExprMap() {
			while (!varID2ExprMap.empty())
				varID2ExprMap.pop_back();

			resetZ3ExprMap();
		}

		// For assert (Q), add ¬Q to the solver to prove the absence of counterexamples;
		// return true means there is no counterexample, false means there is at least one counterexample
		bool checkNegateAssert(z3::expr q) {
			// negative check
			getSolver().push();
			getSolver().add(!q);
			getSolver().check();
			bool res = getSolver().check() == z3::unsat;
			getSolver().pop();
			return res;
		}

	 protected:
		z3::context ctx;
		z3::solver solver;

	 private:
		z3::expr_vector varID2ExprMap;	/// var to z3 expression
		u32_t lastSlot;		/// the last slot in the map for the z3 expression.
	};

} // namespace SVF

#endif // SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H
