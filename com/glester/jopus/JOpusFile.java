package com.glester.jopus;

import java.nio.*;
import javax.sound.sampled.*;

/**
	Represents an Opus-encoded file that can be progressively streamed from disk.
*/
public class JOpusFile extends JOpusDecodable
{
	protected native void jopusOpenFile(String path);
	protected native int jopusRead(ByteBuffer samplesBuffer);
	protected native void jopusClose();

	public JOpusFile(String filePath)
	{
		// after this is called, both "wrapperPointer" and "format" will be set to values.
		jopusOpenFile(filePath);
		setAudioFormatDetails();
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
