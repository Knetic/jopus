#include "com_glester_jopus_JOpusFile.h"
#include <opus/opusfile.h>
#include <stdlib.h>
#include <jni.h>

/*
	Structure to hold the various metadata about an Opus file,
	which can't be held in Java.
*/
typedef struct
{
	OggOpusFile* file;
} OpusWrapper;

void throwException(JNIEnv*, const char*);
void throwOpusException(JNIEnv*, int);

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusFile_jopusOpenFile(JNIEnv* environment, jobject caller, jstring filePath)
{
	OpusWrapper* opus;
	OpusHead* head;
	jclass callerClass;
	jclass formatClass;
	jmethodID formatConstructor;
	jmethodID endiannessMethod;
	jfieldID metaPointerID;
	jfieldID formatID;
	jobject format;
	opus_uint32 sampleRate;
	opus_int32 channels;
	opus_uint32 bitrate;
	int err;
	jboolean bigEndian;

	// set up metadata wrapper.
	err = OPUS_OK;

	opus = malloc(sizeof(OpusWrapper));
	opus->file = op_open_file((*environment)->GetStringUTFChars(environment, filePath, 0), &err); 

	if(err != OPUS_OK)
	{
		throwOpusException(environment, err);
		return;
	}

	head = op_head(opus->file, NULL);

	sampleRate = head->input_sample_rate;
	if(err != OPUS_OK)
	{
		throwOpusException(environment, err);
		return;
	}

	channels = head->channel_count;
	if(err != OPUS_OK)
	{
		throwOpusException(environment, err);
		return;
	}

	bitrate = op_bitrate(opus->file, -1);

	// create AudioFormat
	callerClass	= (*environment)->GetObjectClass(environment, caller);
	formatClass	= (*environment)->FindClass(environment, "javax/sound/sampled/AudioFormat");

	formatConstructor 	= (*environment)->GetMethodID(environment, formatClass, "<init>", "(FIIZZ)V");
	endiannessMethod	= (*environment)->GetMethodID(environment, callerClass, "isNativeOrderBigEndian", "()Z");

	bigEndian 		= (*environment)->CallBooleanMethod(environment, caller, endiannessMethod);
	format 			= (*environment)->NewObject(environment, formatClass, formatConstructor, (jfloat)(sampleRate / channels), 16, channels, JNI_TRUE, bigEndian);

	// set fields in this object
	
	metaPointerID	= (*environment)->GetFieldID(environment, callerClass, "wrapperPointer", "J");
	formatID	= (*environment)->GetFieldID(environment, callerClass, "format", "Ljavax/sound/sampled/AudioFormat;");

	(*environment)->SetLongField(environment, caller, metaPointerID, (jlong)opus);
	(*environment)->SetObjectField(environment, caller, formatID, format);
}

JNIEXPORT jint JNICALL Java_com_glester_jopus_JOpusFile_jopusRead(JNIEnv* environment, jobject caller, jobject sampleBuffer)
{
	OpusWrapper* opus;
	jclass callerClass;
	jfieldID metaPointerID;
	char* sampleBufferContents;
	opus_int32 samplesRead;
	int sampleBufferCapacity;

	// get wrapper data.
	callerClass	= (*environment)->GetObjectClass(environment, caller);
	metaPointerID	= (*environment)->GetFieldID(environment, callerClass, "wrapperPointer", "J");    
	opus 		= (OpusWrapper*)((*environment)->GetLongField(environment, caller, metaPointerID));
	
	sampleBufferContents = (char*)((*environment)->GetDirectBufferAddress(environment, sampleBuffer));
	sampleBufferCapacity = (*environment)->GetDirectBufferCapacity(environment, sampleBuffer);

	// read.
	samplesRead = op_read(opus->file, sampleBufferContents, sampleBufferCapacity, NULL);
	
	return (jint)samplesRead;
}

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusFile_jopusClose(JNIEnv* environment, jobject caller)
{
	OpusWrapper* opus;
	jclass callerClass;
	jfieldID metaPointerID;

	// get wrapper data.
	callerClass	= (*environment)->GetObjectClass(environment, caller);
	metaPointerID	= (*environment)->GetFieldID(environment, callerClass, "wrapperPointer", "J");    
	opus 		= (OpusWrapper*)((*environment)->GetLongField(environment, caller, metaPointerID));

	op_free(opus->file);
	free(opus);
}

/*
	Throws a generic Exception, whose message will be the given [errorMessage].
*/
void throwException(JNIEnv* environment, const char* errorMessage)
{
	jclass exceptionClass;

	exceptionClass = (*environment)->FindClass(environment, "java/lang/Exception");
	(*environment)->ThrowNew(environment, exceptionClass, errorMessage);
}

/*
	Throws an Opus exception, assuming that [error] is an Opus error code.
	https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__errorcodes.html
*/
void throwOpusException(JNIEnv* environment, int error)
{
	const char* errorMessage;

	switch(error)
	{
		case OPUS_ALLOC_FAIL		:	errorMessage = "Unable to create Opus decoder: Allocation failure";
				break;
		case OPUS_INVALID_STATE		:	errorMessage = "Unable to create Opus decoder: Invalid decoder state";
				break;
		case OPUS_UNIMPLEMENTED		:	errorMessage = "Unable to create Opus decoder: Unimplemented request";
				break;
		case OPUS_INVALID_PACKET	:	errorMessage = "Unable to create Opus decoder: Invalid packet";
				break;
		case OPUS_INTERNAL_ERROR	:	errorMessage = "Unable to create Opus decoder: Generic internal error";
				break;
		case OPUS_BUFFER_TOO_SMALL	:	errorMessage = "Unable to create Opus decoder: Buffer too small";
				break;
		case OPUS_BAD_ARG		:	errorMessage = "Unable to create Opus decoder: Bad argument";
				break;
		default				:	errorMessage = "Unable to create Opus decoder: unspecified error";
	}

	throwException(environment, errorMessage);
}

/*
	Checks to determine if the given [error] is an Opus error code.
	Returns JNI_TRUE if so, JNI_FALSE otherwise.
*/
int isOpusError(int error)
{
	switch(error)
	{
		case OPUS_ALLOC_FAIL		:	return JNI_TRUE;
		case OPUS_INVALID_STATE		:	return JNI_TRUE;
		case OPUS_UNIMPLEMENTED		:	return JNI_TRUE;
		case OPUS_INVALID_PACKET	:	return JNI_TRUE;
		case OPUS_INTERNAL_ERROR	:	return JNI_TRUE;
		case OPUS_BUFFER_TOO_SMALL	:	return JNI_TRUE;
		case OPUS_BAD_ARG		:	return JNI_TRUE;
	}
	return JNI_FALSE;
}

/*
	Evidently Opus writes integers in a format that isn't conducive to using a union.
	This converts 4-byte character array into a real int.
*/
int opusCharToInt(char ch[4])
{
	return ((opus_uint32)ch[0]<<24) | ((opus_uint32)ch[1]<<16) | ((opus_uint32)ch[2]<< 8) |  (opus_uint32)ch[3];
}
