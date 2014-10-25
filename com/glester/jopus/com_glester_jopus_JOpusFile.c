#include "com_glester_jopus_JOpusFile.h"
#include "opusfile.h"
#include "jopus.h"
#include <stdlib.h>
#include <jni.h>

JNIEXPORT void JNICALL Java_com_glester_jopus_JOpusFile_jopusOpenFile(JNIEnv* environment, jobject caller, jstring filePath)
{
	OpusWrapper* opus;
	jclass callerClass;
	int err;

	// set up metadata wrapper.
	err = OPUS_OK;

	opus = malloc(sizeof(OpusWrapper));
	opus->file = op_open_file((*environment)->GetStringUTFChars(environment, filePath, 0), &err); 

	if(err != OPUS_OK)
	{
		throwOpusException(environment, err, "Unable to open Opus decoder: ");
		return;
	}

	loadOpusMeta(environment, opus, caller);
}
