#include "com_glester_jopus_JOpusFile.h"
#include "opusfile.h"
#include "jopus.h"
#include <stdlib.h>
#include <jni.h>

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
	int err;
	jboolean bigEndian;

	// set up metadata wrapper.
	err = OPUS_OK;

	opus = malloc(sizeof(OpusWrapper));
	opus->file = op_open_file((*environment)->GetStringUTFChars(environment, filePath, 0), &err); 

	if(err != OPUS_OK)
	{
		throwOpusException(environment, err, "Unable to open Opus decoder: ");
		return;
	}

	head = op_head(opus->file, NULL);

	sampleRate = head->input_sample_rate;
	channels = head->channel_count;

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
