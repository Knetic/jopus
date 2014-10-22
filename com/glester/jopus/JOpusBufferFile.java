package com.glester.jopus;

import java.nio.*;
import javax.sound.sampled.*;
import java.io.*;
import java.util.jar.JarFile;
import java.net.URISyntaxException;

/**
	Represents an Opus-encoded region of memory that can be decoded into PCM.
*/
public class JOpusBufferFile extends JOpusDecodable
{
	private ByteBuffer encodedBuffer;

	protected native void jopusOpenMemory(ByteBuffer encodedBuffer);

	/**
		Opens an Opus file which is fully loaded into [encodedBuffer], and prepares to decode it.
		This method will automatically allocate a sample buffer of the appropriate size;
		if you'd prefer to give this object a buffer to use (instead of having it allocate one)
		then use JOpusFile(String, boolean) to opt out of the sample buffer, and then call
		.setSampleBuffer(ByteBuffer), passing a ByteBuffer which is sized to be .getRequiredBufferSize()
		, or some scale thereof.
	*/
	public JOpusBufferFile(ByteBuffer encodedBuffer)
	{
		this(encodedBuffer, true);
	}

	public JOpusBufferFile(ByteBuffer encodedBuffer, boolean createSampleBuffer)
	{
		this.encodedBuffer = encodedBuffer;
		jopusOpenMemory(encodedBuffer);

		if(createSampleBuffer)
			createSampleBuffer();
	}

	public static ByteBuffer loadBufferFromJar(String path) throws URISyntaxException, IOException, FileNotFoundException
	{
		JarFile currentJar;
		File currentJarFile;
		int size;
		
		currentJarFile = new File(JOpusBufferFile.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath());
		currentJar = new JarFile(currentJarFile);
		size = (int)currentJar.getEntry(path).getSize();

		return loadBufferFromStream(JOpusBufferFile.class.getResourceAsStream(path), size);
	}

	public static ByteBuffer loadBufferFromFile(String path) throws URISyntaxException, IOException, FileNotFoundException
	{
		File encodedFile;
		FileInputStream fileStream;

		encodedFile = new File(path);
		fileStream = new FileInputStream(encodedFile);
	
		return loadBufferFromStream(fileStream, (int)encodedFile.length());
	}

	protected static ByteBuffer loadBufferFromStream(InputStream stream, int size) throws IOException
	{
		ByteBuffer encodedBuffer;
		byte[] copyBuffer;
		int bytesRead;

		encodedBuffer = ByteBuffer.allocateDirect(size).order(ByteOrder.nativeOrder());
		copyBuffer = new byte[1024];

		while((bytesRead = (int)stream.read(copyBuffer)) > 0)
			encodedBuffer.put(copyBuffer, 0, bytesRead);
		
		return encodedBuffer;	
	}

	public static JOpusBufferFile loadFromJar(String path) throws URISyntaxException, IOException, FileNotFoundException
	{
		return new JOpusBufferFile(loadBufferFromJar(path));
	}

	public static JOpusBufferFile loadFromFile(String path) throws URISyntaxException, IOException, FileNotFoundException
	{
		return new JOpusBufferFile(loadBufferFromFile(path));
	}

	public int read()
	{
		int samplesRead;

		samplesRead = jopusRead(sampleBuffer);
		sampleBuffer.position(0);

		// number of samples times 
		return samplesRead * 2 * format.getChannels();
	}

	public void close()
	{
		jopusClose();
	}	

	static
	{
		System.loadLibrary("jopus");
	}
}
