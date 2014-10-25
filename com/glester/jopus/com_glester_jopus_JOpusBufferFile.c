#include "com_glester_jopus_JOpusBufferFile.h"
#include "opusfile.h"
#include "jopus.h"

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusBufferFile_jopusOpenMemory(JNIEnv* environment, jobject caller, jobject encodedBuffer)
{
	OpusWrapper* opus;
	jclass callerClass;
	void* encodedBufferContents;
	jlong encodedBufferCapacity;
	int err;

	// set up metadata wrapper.
	err = OPUS_OK;

	opus = malloc(sizeof(OpusWrapper));

	encodedBufferCapacity = (*environment)->GetDirectBufferCapacity(environment, encodedBuffer);
	encodedBufferContents = (*environment)->GetDirectBufferAddress(environment, encodedBuffer);

	opus->file = op_open_memory(encodedBufferContents, encodedBufferCapacity, &err); 

	if(err != OPUS_OK)
	{
		throwOpusException(environment, err, "Unable to load Opus from buffer");
		return;
	}

	loadOpusMeta(environment, opus, caller);
}
