package com.glester.jopus;

import java.nio.*;
import javax.sound.sampled.*;

public abstract class JOpusDecodable
{
	public ByteBuffer sampleBuffer;
	public AudioFormat format;

	/**
		Holds the actual memory location of the OggOpusFile object in memory.
		Used by native code when reading or closing the file.
	*/
	protected long wrapperPointer;
	protected int sampleSizeInBytes;

	public abstract int read();
	public abstract void close();

	protected void setAudioFormatDetails()
	{
		int bufferSize;

		// one 16-bit sample per channel, with enough room for .2s of sound per buffer.
		bufferSize = 16 * format.getChannels() * (int)(format.getSampleRate() / 4);
		
		sampleBuffer = ByteBuffer.allocateDirect(bufferSize).order(ByteOrder.nativeOrder());
		sampleSizeInBytes = format.getSampleSizeInBits() / 8;
	}

	/**
		Used by native code when constructing the AudioFormat.
	*/
	protected boolean isNativeOrderBigEndian()
	{
		return ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN;
	}
}
