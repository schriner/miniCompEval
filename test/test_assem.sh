
#
#!/usr/bin/env bash
# miniCompEval
# Sonya Schriner
# File: test_assem.sh
#

TEST_PATH=${0%/*}
BUILD_DIR=$TEST_PATH/../build
TEST_DIR=$TEST_PATH/Compiler
OUT_DIR=$TEST_PATH/CompilerOut

echo "Checking for exe"
if [ -d "$BUILD" ]; then
	pushd $TEST_PATH/..
	cmake -B build
	cmake --build build
	popd
fi

echo "Building Docker Container for Testing"
docker build -t armv7_s $TEST_PATH/.. 

for entry in "$TEST_DIR"/*".java"
do
	file=$(basename -- "$entry")
	echo "$entry" $OUT_DIR/$file "${file%.*}.s"
	./$BUILD_DIR/mjavac -o $OUT_DIR/"${file%.*}.s" "$entry" 
	docker run -it --rm armv7_s bash ./comp.sh "${file%.*}"
done

