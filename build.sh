#!/bin/bash

# Pulls and builds the Opus/Opusfile source,
# then builds jopus.

# This script ought to be platform independent, but requires the use of 'git', 'gcc', 'java', 'jar', 'make', and 'dh-autoreconf'.
# No attempt is made by the script to install these tools, you'll have to do it yourself.

# before anything else, make sure JAVA_HOME is specified. We need this for the JNI shared libraries.
if [ "$JAVA_HOME" == "" ]
then
	echo "Environment variable JAVA_HOME must be specified."
	echo "This should point to the path containing a Java JDK, complete with an 'includes' folder."
	echo "For more information, see http://docs.oracle.com/cd/E19182-01/820-7851/inst_cli_jdk_javahome_t/index.html"
	exit;
fi

function checkError()
{
	EXITCODE=$?

	if [ $EXITCODE -ne 0 ];
	then
		echo "Exit code of previous command indicated an error: $EXITCODE"
		exit $EXITCODE
	fi

}

function buildOpus()
{
	if [ -f ./autogen.sh ]
	then
		./autogen.sh
		checkError
	fi

	./configure
	checkError

	make
	checkError

	sudo make install
	checkError
}

# pull ogg
if [ ! -d ./ogg ]
then
	wget -c http://downloads.xiph.org/releases/ogg/libogg-1.3.2.tar.gz

	tar xzvf ./libogg-1.3.2.tar.gz
	mv ./libogg-1.3.2 ./ogg
	rm ./libogg-1.3.2.tar.gz

	pushd ./ogg >> /dev/null
	buildOpus
	popd >> /dev/null
fi

# pull opus
if [ ! -d ./opus ]
then
	git clone git://git.opus-codec.org/opus.git
	checkError

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
		checkError
		buildOpus
	fi

	popd >> /dev/null
fi

# pull opusfile
if [ ! -d ./opusfile ]
then

	git clone git://git.xiph.org/opusfile.git
	checkError

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
		checkError
		buildOpus
	fi

	popd >> /dev/null
fi

# clean.
echo "Cleaning files"
rm -f *.so
rm -f *.java~
rm -f *.c~
rm -f *.h~
rm -f *.sh~
rm -f *.log

# prepare jni headers and compile wrapper class
echo "Building java file"
javac ./com/glester/jopus/JOpusFile.java
checkError

javah -jni -d ./com/glester/jopus com.glester.jopus.JOpusFile
checkError

# compile jopus native
echo "Compiling jni library"

gcc -shared ./com/glester/jopus/*.c \
	-I./ogg/include -I./opus/include -I./opusfile/include \
	-I$JAVA_HOME/include -I$JAVA_HOME/include/linux \
	-L./opus/.libs -L./opusfile/.libs \
	-lopus -lopusfile \
	-o ./libjopus.so \
	-w -fPIC -m64
checkError

cp ./ogg/src/.libs/libogg.so ./libogg.so.0
cp ./opus/.libs/libopus.so ./libopus.so.0
cp ./opusfile/.libs/libopusfile.so ./libopusfile.so.0

# make jar
echo "Zipping"
jar cf jopus.jar ./META-INF/* ./com/glester/jopus/*.class
checkError

echo "Generating docs"
javadoc com.glester.jopus -d ./doc

echo "Done"
