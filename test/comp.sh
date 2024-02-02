
#!/usr/bin/env bash

gcc /CompilerOut/$1.s -o CompilerOut/$1
cd /Compiler
javac $1.java
cd /

echo
echo "Beginning Test: $1"
diff <(cd /Compiler && java $1) <(./CompilerOut/$1) > /dev/null 2>&1
error=$?
if [ $error -eq 0 ]
then
	echo "Success"
elif [ $error -eq 1 ]
then
	echo "Test Failure: $1"
	pushd /Compiler
	java $1
	popd
	echo "Expected: "
	./CompilerOut/$1
	echo "Actual $1: "
else
	echo "There was something wrong with the diff command"
fi
echo
