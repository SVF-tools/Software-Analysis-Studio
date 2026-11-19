//===- Z3Mgr.h -- Z3 manager for symbolic execution ------------------//
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
 * Z3 manager for symbolic execution
 *
 * Created on: Feb 19, 2024
 */

#ifndef SOFTWARE_SECURITY_ANALYSIS_Z3SSEMGR_H
#define SOFTWARE_SECURITY_ANALYSIS_Z3SSEMGR_H

#include "Z3Mgr.h"
#include "Graphs/PAG.h"

namespace SVF {

	class PAG;
	class ValVar;
	class ObjVar;
	class GepStmt;

	class Z3SSEMgr : public Z3Mgr {
		typedef std::vector<const ICFGNode*> CallStack;
	 public:
		/// Constructor
		Z3SSEMgr(PAG* ir);


		std::string callingCtxToStr(const CallStack& callingCtx);

		z3::expr getZ3Expr(u32_t idx, const CallStack& callingCtx);

		/// Initialize the expr value for each objects (address-taken variables and constants)
		z3::expr createExprForObjVar(const ObjVar* obj);

		/// Return the address expr of a ObjVar
		z3::expr getMemObjAddress(u32_t idx);

		/// Return the field address given a pointer points to a struct object and an offset
		z3::expr getGepObjAddress(z3::expr pointer, u32_t offset);

		/// Return the offset expression of a GepStmt
		s32_t getGepOffset(const GepStmt* gep, const CallStack& callingCtx);

		/// Dump values of all exprs
		virtual void printExprValues(const CallStack& callingCtx);

	 private:
		PAG* svfir;
	};

} // namespace SVF

#endif // SOFTWARE_SECURITY_ANALYSIS_Z3MGR_H
