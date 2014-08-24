#!/bin/bash

# Pulls and builds the Opus/Opusfile source,
# then builds jopus.

# before anything else, make sure JAVA_HOME is specified. We need this for the JNI shared libraries.
if [ "$JAVA_HOME" == "" ]
then
	echo "Environment variable JAVA_HOME must be specified."
	echo "This should point to the path containing a Java JDK, complete with an 'includes' folder."
	echo "For more information, see http://docs.oracle.com/cd/E19182-01/820-7851/inst_cli_jdk_javahome_t/index.html"
	exit;
fi

function buildOpus()
{
	./autogen.sh
	./configure
	make
}

# pull opus
if [ ! -d ./opus ]
then
	git clone git://git.opus-codec.org/opus.git

	pushd opus >> /dev/null
	buildOpus
	popd >> /dev/null
else
	# build
	pushd opus >> /dev/null

	git remote update
	LOCAL=$(git rev-parse @)
	BASE=$(git merge-base @ @{u})

	if [ $LOCAL != $BASE ]; then
		git pull origin master
		buildOpus
	fi

	popd >> /dev/null
fi

# pull opusfile
if [ ! -d ./opusfile ]
then

	git clone git://git.xiph.org/opusfile.git
	pushd opusfile >> /dev/null
	buildOpus
	popd >> /dev/null
else

	# build
	pushd opusfile >> /dev/null

	git remote update
	LOCAL=$(git rev-parse @)
	BASE=$(git merge-base @ @{u})

	if [ $LOCAL != $BASE ]; then
		git pull origin master
		buildOpus
	fi

	popd >> /dev/null
fi

# clean.
echo "Cleaning files"
rm -f libjopus.so
rm -f *.java~
rm -f *.c~
rm -f *.h~
rm -f *.sh~
rm -f *.log

# prepare jni headers and compile wrapper class
echo "Building java file"
javac ./com/glester/jopus/JOpusFile.java
javah -jni -d ./com/glester/jopus com.glester.jopus.JOpusFile

# compile jopus native
echo "Compiling jni library"

gcc -shared ./com/glester/jopus/*.c \
	-I./opus/include -I./opusfile/include \
	-I$JAVA_HOME/include -I$JAVA_HOME/include/linux \
	-L./opus/.libs -L./opusfile/.libs \
	-lopus -lopusfile \
	-o ./libjopus.so \
	-w -fPIC -m64

cp ./opus/.libs/libopus.so .
cp ./opusfile/.libs/libopusfile.so .

echo "Done"
