#include "jopus.h"

void loadOpusMeta(JNIEnv* environment, OpusWrapper* opus, jobject caller)
{
	OpusHead* head;
	jclass formatClass;
	jclass callerClass;
	jmethodID formatConstructor;
	jmethodID endiannessMethod;
	jfieldID metaPointerID;
	jfieldID formatID;
	jfieldID uncompressedLengthID;
	jfieldID bitrateID;
	jfieldID versionID;
	jfieldID streamCountID;
	jobject format;
	opus_uint32 sampleRate;
	opus_int32 channels;
	jboolean bigEndian;

	loadOpusComments(environment, opus->file, caller);
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
	uncompressedLengthID	= (*environment)->GetFieldID(environment, callerClass, "uncompressedLength", "J");
	bitrateID	= (*environment)->GetFieldID(environment, callerClass, "bitrate", "I");
	versionID	= (*environment)->GetFieldID(environment, callerClass, "opusVersion", "I");
	streamCountID	= (*environment)->GetFieldID(environment, callerClass, "streamCount", "I");

	(*environment)->SetLongField(environment, caller, metaPointerID, (jlong)opus);
	(*environment)->SetObjectField(environment, caller, formatID, format);
	(*environment)->SetLongField(environment, caller, uncompressedLengthID, op_pcm_total(opus->file, -1));
	(*environment)->SetIntField(environment, caller, bitrateID, op_bitrate(opus->file, -1));
	(*environment)->SetIntField(environment, caller, versionID, head->version);
	(*environment)->SetIntField(environment, caller, streamCountID, head->stream_count);
}

/*
	Loads the "tags" for an OpusFile.
*/
void loadOpusComments(JNIEnv* environment, OggOpusFile* file, jobject caller)
{
	jclass callerClass;
	jclass stringClass;
	jclass opusTagClass;
	jclass listClass;
	jmethodID listAddID;
	jmethodID tagCtorID;
	jfieldID commentsID;
	jfieldID tagsID;
	jfieldID vendorID;
	OpusTags* tags;
	jobject tagsList;
	jobject commentsList;
	jobject tag;
	jstring* comment;
	jstring vendorString;

	callerClass 		= (*environment)->GetObjectClass(environment, caller);
	stringClass		= (*environment)->FindClass(environment, "java/lang/String");
	listClass		= (*environment)->FindClass(environment, "java/util/List");
	opusTagClass		= (*environment)->FindClass(environment, "com/glester/jopus/OpusTag");
	commentsID	 	= (*environment)->GetFieldID(environment, callerClass, "comments", "Ljava/util/List;");
	tagsID	 		= (*environment)->GetFieldID(environment, callerClass, "tags", "Ljava/util/List;");
	vendorID 		= (*environment)->GetFieldID(environment, callerClass, "vendor", "Ljava/lang/String;");
	listAddID		= (*environment)->GetMethodID(environment, listClass, "add", "(Ljava/lang/Object;)Z");	
	tagCtorID		= (*environment)->GetMethodID(environment, opusTagClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");	

	commentsList 		= (*environment)->GetObjectField(environment, caller, commentsID);
	tagsList 		= (*environment)->GetObjectField(environment, caller, tagsID);

	tags = op_tags(file, -1);

	for(int i = 0; i < tags->comments; i++)
	{
		comment = getStringPair(environment, tags->user_comments[i], tags->comment_lengths[i]);

		if(comment[1] == NULL)
			(*environment)->CallBooleanMethod(environment, commentsList, listAddID, comment[0]);
		else
		{
			tag = (*environment)->NewObject(environment, opusTagClass, tagCtorID, comment[0], comment[1]);
			(*environment)->CallBooleanMethod(environment, tagsList, listAddID, tag);
		}
	}

	vendorString = getNullTerminatedString(environment, tags->vendor);
	(*environment)->SetObjectField(environment, caller, vendorID, vendorString);
}

jstring getNullTerminatedString(JNIEnv* environment, char* characters)
{
	jchar* utfChars;
	int len;

	len = -1;

	do
		len++;
	while(characters[len] != 0);

	utfChars = (jchar*)calloc(sizeof(jchar), len);

	for(int i = 0; i < len; i++)
		utfChars[i] = (jchar)characters[i];

	return (*environment)->NewString(environment, utfChars, len);
}

jstring* getStringPair(JNIEnv* environment, char* characters, int len)
{
	jchar* utfChars;
	jstring ret[2];
	int splitIndex;
	int stringLength;

	splitIndex = -1;

	for(int i = 0; i < len; i++)
	{
		if(characters[i] == '=')
		{
			splitIndex = i;
			break;
		}
	}

	if(splitIndex == -1)
	{
		ret[0] = getNullTerminatedString(environment, characters);
		ret[1] = NULL;
		return ret;
	}

	// get key name
	utfChars = (jchar*)calloc(sizeof(jchar), splitIndex);

	for(int i = 0; i < splitIndex; i++)
		utfChars[i] = (jchar)characters[i];
	ret[0] = (*environment)->NewString(environment, utfChars, splitIndex);

	// get key value
	stringLength = len - splitIndex - 1;
	utfChars = (jchar*)calloc(sizeof(jchar), stringLength);

	for(int i = 0; i < stringLength; i++)
		utfChars[i] = (jchar)characters[i + splitIndex + 1];
	ret[1] = (*environment)->NewString(environment, utfChars, stringLength);
	return ret;
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
