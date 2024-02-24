
#
#!/usr/bin/env bash
# miniCompEval
# Sonya Schriner
# File: test_llvm.sh 
#

if [ ! -f $1 ]; then
	echo "Test Failure: $1 does not exist"
	exit -1
fi

TEST_PATH=${0%/*}
TEST_DIR=${1%/*}
OUT_DIR=$TEST_PATH/tmpLLVM_IR
BUILD_DIR=$TEST_PATH/../build
file=$(basename -- "$1")

mkdir $OUT_DIR

$BUILD_DIR/mjavacllvm -o $OUT_DIR/"${file%.*}.bc" $1
if [ ! -f $OUT_DIR/"${file%.*}.bc" ]; then
	echo
	echo "Test Failure: $OUT_DIR/${file%.*}.bc was not built"
	rm -r $OUT_DIR
	exit -1
fi
clang $OUT_DIR/${file%.*}.bc -o $OUT_DIR/${file%.*}
javac $1

echo
pushd $TEST_DIR > /dev/null 3>&1
if [ ! -f "${file%.*}.class" ]; then
	echo
	echo "Test Failure: ${file%.*}.class was not built"
	rm -r $OUT_DIR
	exit -1
fi
java ${file%.*} > tmpjavac
rm *.class
popd > /dev/null 2>&1
$OUT_DIR/${file%.*} > tmpllvm

diff $TEST_DIR/tmpjavac tmpllvm > /dev/null 2>&1
error=$?
if [ $error -eq 0 ]
then
	echo "Success"
	rm -r $OUT_DIR 
	rm tmpllvm $TEST_DIR/tmpjavac
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


