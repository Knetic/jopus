package com.glester.jopus;

import java.nio.*;
import java.util.*;
import javax.sound.sampled.*;

public abstract class JOpusDecodable
{
	public AudioFormat format;
	protected ByteBuffer sampleBuffer;	

	/**
		Holds the actual memory location of the OggOpusFile object in memory.
		Used by native code when reading or closing the file.
	*/
	protected long wrapperPointer;
	protected int sampleSizeInBytes;
	protected long uncompressedLength;
	protected int bitrate;
	protected int opusVersion;
	protected int streamCount;

	private List<OpusTag> tags;
	private List<String> comments;
	private String vendor;
	
	public abstract int read();
	public abstract void close();

	protected native int jopusRead(ByteBuffer samplesBuffer);
	protected native void jopusClose();

	protected JOpusDecodable()
	{
		tags = new LinkedList<OpusTag>();
		comments = new LinkedList<String>();
	}

	public void setSampleBuffer(ByteBuffer buffer)
	{
		if(buffer == null)
			throw new IllegalArgumentException("Given buffer cannot be null");
		if(!buffer.isDirect())			
			throw new IllegalArgumentException("Given buffer must be direct");
		
		sampleBuffer = buffer;
	}

	/**
		Returns the size of buffer required to buffer one second of decoded output.
		A sample buffer may be smaller than this size (the default sample buffer holds .25s of decoded output)
		But any sample buffer given to this object should sized to be a scaled version of whatever 
		number this method returns.
	*/
	public int getRequiredBufferSize()
	{
		return 16 * format.getChannels() * (int)(format.getSampleRate());
	}

	public ByteBuffer getSampleBuffer()
	{
		return sampleBuffer;
	}
	
	public Iterable<String> getComments()
	{
		return comments;
	}

	public Iterable<String> getTag(String key)
	{
		List<String> ret;

		ret = new LinkedList<String>();

		for(OpusTag tag : tags)
			if(tag.getKey().equals(key))
				ret.add(tag.getValue());

		return ret;
	}

	public Iterable<OpusTag> getTags()
	{
		return tags;
	}
	
	public String getVendor()
	{
		return vendor;
	}

	public long getUncompressedLength()
	{
		return uncompressedLength;
	}

	public int getBitrate()
	{
		return bitrate;
	}

	public int getVersion()
	{
		return opusVersion;
	}

	public int getStreamCount()
	{
		return streamCount;
	}

	protected void createSampleBuffer()
	{
		sampleBuffer = ByteBuffer.allocateDirect(getRequiredBufferSize() / 4).order(ByteOrder.nativeOrder());
	}

	protected void determineSampleSize()
	{		
		sampleSizeInBytes = format.getSampleSizeInBits() / 8;
	}

	/**
		Used by native code when constructing the AudioFormat.
	*/
	protected boolean isNativeOrderBigEndian()
	{
		return ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN;
	}

	static
	{
		System.loadLibrary("jopus");
	}
}
