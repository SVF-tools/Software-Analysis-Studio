#!/bin/bash

# Get the current working directory and store it in PROJECTHOME
PROJECTHOME=$(pwd)

# Detect the operating system type and store it in sysOS
sysOS=`uname -s`

# Set the major version of LLVM
MajorLLVMVer=22

# Define the full LLVM version
LLVMVer=${MajorLLVMVer}.1.0

# Set the home directories for LLVM and Z3
LLVMHome="llvm-${LLVMVer}.obj"
Z3Home="z3.obj"

# Change this to your SVF root directory
svf_root=`pwd`/../SVF

# Export the paths for the LLVM, Z3, and SVF directories
export LLVM_DIR=$svf_root/$LLVMHome
export Z3_DIR=$svf_root/$Z3Home
export SVF_DIR=$svf_root

# Update the PATH to include the binary directories for SVF, LLVM, and the project
export PATH=$SVF_DIR/Release-build/bin:$PATH
export PATH=$LLVM_DIR/bin:$PATH
export PATH=$PROJECTHOME/bin:$PATH

# Print the paths to the terminal for verification
echo "SVF_DIR="$SVF_DIR
echo "LLVM_DIR="$LLVM_DIR
echo "Z3_DIR="$Z3_DIR
