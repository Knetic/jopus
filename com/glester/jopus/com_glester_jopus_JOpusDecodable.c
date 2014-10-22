#include "com_glester_jopus_JOpusDecodable.h"
#include "opusfile.h"
#include "jopus.h"

JNIEXPORT jint JNICALL Java_com_glester_jopus_JOpusDecodable_jopusRead(JNIEnv* environment, jobject caller, jobject sampleBuffer)
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

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusDecodable_jopusClose(JNIEnv* environment, jobject caller)
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
