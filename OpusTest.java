import javax.sound.sampled.*;
import java.nio.*;
import com.glester.jopus.*;

public class OpusTest
{
	private static Mixer mixer;
	private static SourceDataLine line;
	private static JOpusDecodable opus;

	public static void main(String[] args)
	{
		try
		{
			opus = JOpusBufferFile.loadFromFile("jazz.opus");
			printComments();

			selectLine();

			if(line != null)
				play();

			opus.close();
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}

	public static void printComments()
	{
		System.out.println("Vendor: '"+opus.getVendor()+"'");
		for(String comment : opus.getComments())
			System.out.println(comment);
	}
	
	public static void selectLine()
	{
		Line.Info[] sources;
		DataLine.Info info;

		try
		{
			info = new DataLine.Info(SourceDataLine.class, opus.format);

			if(AudioSystem.isLineSupported(info))
			{
				System.out.println("Found a line");
				line = (SourceDataLine)AudioSystem.getLine(info);
				line.open(opus.format, opus.getSampleBuffer().capacity());
				line.start();
			}
			else
				System.out.println("No line found");		

		}
		catch (Exception ex) 
		{
			ex.printStackTrace();
			return;
		}
	}

	public static void play()
	{
		ByteBuffer sampleBuffer;
		byte[] backingBuffer;
		int bytesRead;

		sampleBuffer = opus.getSampleBuffer();
		backingBuffer = new byte[sampleBuffer.capacity()];

		while(true)
		{
			bytesRead = opus.read();

			if(bytesRead <= 0)
					break;

			sampleBuffer.get(backingBuffer);		
			line.write(backingBuffer, 0, bytesRead);
		}
	}
}
