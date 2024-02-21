
#
#!/usr/bin/env bash
# miniCompEval
# Sonya Schriner
# File: test_assem.sh
#

TEST_PATH=${0%/*}
BUILD_DIR=$TEST_PATH/../build
TEST_DIR=$TEST_PATH/CodeGen
OUT_DIR=$TEST_PATH/ARMv7Out

echo "Checking for exe"
if [ -d "$BUILD" ]; then
	pushd $TEST_PATH/..
	cmake -B build
	cmake --build build
	popd
fi

for entry in "$TEST_DIR"/*".java"
do
	file=$(basename -- "$entry")
	echo "	./$BUILD_DIR/mjavac -o $OUT_DIR/${file%.*}.s $entry"
	./$BUILD_DIR/mjavac -o $OUT_DIR/"${file%.*}.s" "$entry" 
done

echo "Building Docker Container for Testing"
docker build -t armv7_s $TEST_PATH/. 

for entry in "$TEST_DIR"/*".java"
do
	file=$(basename -- "$entry")
	docker run -it --rm armv7_s bash ./comp.sh "${file%.*}"
done

