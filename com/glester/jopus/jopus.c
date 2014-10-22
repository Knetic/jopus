#include "jopus.h"

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
void throwOpusException(JNIEnv* environment, int error, const char* message)
{
	const char* errorMessage;
	char resultMessage[256];

	switch(error)
	{
		case OPUS_ALLOC_FAIL		:	errorMessage = "Allocation failure";
				break;
		case OPUS_INVALID_STATE		:	errorMessage = "Invalid decoder state";
				break;
		case OPUS_UNIMPLEMENTED		:	errorMessage = "Unimplemented request";
				break;
		case OPUS_INVALID_PACKET	:	errorMessage = "Invalid packet";
				break;
		case OPUS_INTERNAL_ERROR	:	errorMessage = "Generic internal error";
				break;
		case OPUS_BUFFER_TOO_SMALL	:	errorMessage = "Buffer too small";
				break;
		case OPUS_BAD_ARG		:	errorMessage = "Bad argument";
				break;
		case OP_ENOTFORMAT		:	errorMessage = "Not an Opus stream";
				break;
		case OP_EBADHEADER		:	errorMessage = "Bad header";
				break;
		case OP_EVERSION		:	errorMessage = "Version mismatch";
				break;
		default				:	errorMessage = "unspecified error";
	}

	strcat(resultMessage, message);
	strcat(resultMessage, errorMessage);

	throwException(environment, resultMessage);
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
