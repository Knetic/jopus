package com.glester.jopus;

import java.nio.*;
import javax.sound.sampled.*;

/**
	Represents an Opus-encoded region of memory that can be decoded into PCM.
*/
public class JOpusBufferFile extends JOpusDecodable
{
	private ByteBuffer encodedBuffer;

	protected native void jopusOpenMemory(ByteBuffer encodedBuffer);
	protected native int jopusReadMemory(ByteBuffer samplesBuffer);
	protected native void jopusCloseMemory();

	public JOpusBufferFile(ByteBuffer encodedBuffer)
	{
		this.encodedBuffer = encodedBuffer;
		jopusOpenMemory(encodedBuffer);
		setAudioFormatDetails();
	}

	public int read()
	{
		int samplesRead;

		samplesRead = jopusReadMemory(sampleBuffer);
		sampleBuffer.position(0);

		// number of samples times 
		return samplesRead * 2 * format.getChannels();
	}

	public void close()
	{
		jopusCloseMemory();
	}	
}
