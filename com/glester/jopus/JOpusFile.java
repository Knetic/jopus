package com.glester.jopus;

import java.nio.*;
import javax.sound.sampled.*;

/**
	Represents an Opus-encoded file that can be progressively streamed from disk.
*/
public class JOpusFile extends JOpusDecodable
{
	protected native void jopusOpenFile(String path);

	/**
		Opens an Opus file at the given [filePath], and prepares to decode it.
		This method will automatically allocate a sample buffer of the appropriate size;
		if you'd prefer to give this object a buffer to use (instead of having it allocate one)
		then use JOpusFile(String, boolean) to opt out of the sample buffer, and then call
		.setSampleBuffer(ByteBuffer), passing a ByteBuffer which is sized to be .getRequiredBufferSize()
		, or some scale thereof.
	*/
	public JOpusFile(String filePath)
	{
		this(filePath, true);
	}	

	public JOpusFile(String filePath, boolean createSampleBuffer)
	{
		super();
		
		jopusOpenFile(filePath);
		determineSampleSize();

		if(createSampleBuffer)
			createSampleBuffer();
	}

	/**
		Reads and decodes a length of samples from the file. Returns the number of decoded PCM bytes that resulted.
		The actual contents of the read operation will be in this.sampleBuffer
	*/
	public int read()
	{
		int samplesRead;

		samplesRead = jopusRead(sampleBuffer);
		sampleBuffer.position(0);

		// number of samples times 
		return samplesRead * 2 * format.getChannels();
	}

	/**
		Closes any file handles and disposes of any resources used by this object.
	*/
	public void close()
	{
		jopusClose();
	}

	static
	{
		System.loadLibrary("jopus");
	}
}
