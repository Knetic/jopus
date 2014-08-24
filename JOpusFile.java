import java.nio.*;
import javax.sound.sampled.*;

public class JOpusFile
{
	public ByteBuffer sampleBuffer;
	public long wrapperPointer;

	// file information
	public AudioFormat format;

	public int sampleRate;
	public int bitrate;
    	public int channels;

	//
	public native void jopusOpenFile(String path);
	public native int jopusRead(ByteBuffer samplesBuffer);
	public native void jopusClose();

	public JOpusFile(String filePath)
	{
		jopusOpenFile(filePath);

		sampleBuffer = ByteBuffer.allocateDirect(sampleRate * channels).order(ByteOrder.nativeOrder());
		format = new AudioFormat((float)sampleRate, 16, channels, true, ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN);

		System.out.println("Sample rate: " + sampleRate);
		System.out.println("Bitrate: " + bitrate);
	}	

	public int read()
	{
		int samplesRead;

		samplesRead = jopusRead(sampleBuffer);
		sampleBuffer.position(0);

		return samplesRead * 16 / channels;
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
