#include "com_glester_jopus_JOpusBufferFile.h"
#include "opusfile.h"
#include "jopus.h"

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusBufferFile_jopusOpenMemory(JNIEnv* environment, jobject caller, jobject encodedBuffer)
{
	OpusMemoryWrapper* opus;
	OpusHead* head;
	jclass callerClass;
	jclass formatClass;
	jmethodID formatConstructor;
	jmethodID endiannessMethod;
	jfieldID metaPointerID;
	jfieldID formatID;
	jobject format;
	jlong* encodedBufferContents;
	opus_uint32 sampleRate;
	opus_int32 channels;
	opus_uint32 bitrate;
	jlong encodedBufferCapacity;
	int err;
	jboolean bigEndian;

	// set up metadata wrapper.
	err = OPUS_OK;

	opus = malloc(sizeof(OpusMemoryWrapper));
	
	encodedBufferCapacity = (*environment)->GetDirectBufferCapacity(environment, encodedBuffer);
	encodedBufferContents = (*environment)->GetByteArrayElements(environment, NULL);

	op_mem_stream_create(opus->callbacks, encodedBufferContents, encodedBufferCapacity); 

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

JNIEXPORT jint JNICALL Java_com_glester_jopus_JOpusBufferFile_jopusReadMemory(JNIEnv* environment, jobject caller, jobject sampleBuffer)
{
	return 0;
}

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusBufferFile_jopusCloseMemory(JNIEnv* environment, jobject caller)
{
	
}
