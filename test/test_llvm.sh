
#
#!/usr/bin/env bash
# miniCompEval
# Sonya Schriner
# File: test_llvm.sh 
#

TEST_PATH=${0%/*}
TEST_DIR=${1%/*}
OUT_DIR=$TEST_PATH/LLVM_IR
BUILD_DIR=$TEST_PATH/../build
file=$(basename -- "$1")

./$BUILD_DIR/mjavacllvm -o $OUT_DIR/"${file%.*}.bc" $1
clang $OUT_DIR/${file%.*}.bc -o $OUT_DIR/${file%.*}
javac $1

echo
pushd $TEST_DIR > /dev/null 3>&1
java ${file%.*} > tmpjavac
popd > /dev/null 2>&1
$OUT_DIR/${file%.*} > tmpllvm

diff $TEST_DIR/tmpjavac tmpllvm > /dev/null 2>&1
error=$?
if [ $error -eq 0 ]
then
	echo "Success"
	rm $OUT_DIR/${file%.*} tmpllvm $TEST_DIR/tmpjavac
elif [ $error -eq 1 ]
then
	echo "Test Failure: ${file%.*}"
	pushd "$TEST_DIR"
	java $file
	popd
	echo "Expected: "
	$OUT_DIR/${file%.*}
	echo "Actual ${file%.*}: "
	exit -1
else
	echo "There was something wrong with the diff command"
fi


