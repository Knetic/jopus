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

void loadOpusMeta(JNIEnv*, OpusWrapper*, jobject);

jstring getNullTerminatedString(JNIEnv*, char*);
jstring* getStringPair(JNIEnv*, char*, int);

void throwException(JNIEnv*, const char*);
void throwOpusException(JNIEnv*, int, const char*);
