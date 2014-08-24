import java.nio.*;
import javax.sound.sampled.*;

public class JOpusFile
{
	public ByteBuffer sampleBuffer;
	public AudioFormat format;

	protected long wrapperPointer;
	private int sampleSizeInBytes;

	//
	protected native void jopusOpenFile(String path);
	protected native int jopusRead(ByteBuffer samplesBuffer);
	protected native void jopusClose();

	public JOpusFile(String filePath)
	{
		int bufferSize;

		// after this is called, both "wrapperPointer" and "format" will be set to values.
		jopusOpenFile(filePath);

		// one 16-bit sample per channel, with enough room for .2s of sound per buffer.
		bufferSize = 16 * format.getChannels() * (int)(format.getSampleRate() / 4);
		
		sampleBuffer = ByteBuffer.allocateDirect(bufferSize).order(ByteOrder.nativeOrder());
		sampleSizeInBytes = format.getSampleSizeInBits() / 8;
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

	protected boolean isNativeOrderBigEndian()
	{
		return ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN;
	}

	static
	{
		System.loadLibrary("jopus");
	}
}
