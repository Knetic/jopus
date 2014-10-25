#include "opusfile.h"
#include <jni.h>
#include <stdlib.h>

/*
	Structure to hold the various metadata about an Opus file,
	which can't be held in Java.
*/
typedef struct
{
	OggOpusFile* file;
} OpusWrapper;

void loadOpusComments(JNIEnv*, OggOpusFile*, jobject);
jstring getJString(JNIEnv*, char*, int);

void throwException(JNIEnv*, const char*);
void throwOpusException(JNIEnv*, int, const char*);
