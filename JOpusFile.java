import java.nio.*;
import javax.sound.sampled.*;

public class JOpusFile
{
	public ByteBuffer sampleBuffer;
	public AudioFormat format;

	// raw format information. These are all converted to AudioFormat during construction.
	// TODO: make these a tiny private class, so that we don't hold more memory than we need to.
	protected long wrapperPointer;
	protected int sampleRate;
	protected int bitrate;
    	protected int channels;

	private int sampleSizeInBytes;

	//
	protected native void jopusOpenFile(String path);
	protected native int jopusRead(ByteBuffer samplesBuffer);
	protected native void jopusClose();

	public JOpusFile(String filePath)
	{
		jopusOpenFile(filePath);

		// one 16-bit sample per channel, with enough room for .2s of sound per buffer.
		sampleBuffer = ByteBuffer.allocateDirect(16 * channels * (sampleRate / 4)).order(ByteOrder.nativeOrder());
		format = new AudioFormat((float)sampleRate / channels, 16, channels, true, ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN);

		sampleSizeInBytes = format.getSampleSizeInBits() / 8;
	}	

	public int read()
	{
		int samplesRead;

		samplesRead = jopusRead(sampleBuffer);
		sampleBuffer.position(0);

		// number of samples times 
		return samplesRead * 2 * channels;
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
