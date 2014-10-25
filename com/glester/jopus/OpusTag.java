package com.glester.jopus;

/**
	Tags are Opus comments that contain no spaces before an '=' character. e.g., "TITLE=my awesome track" would parse to "TITLE": "my awesome track".
*/
public class OpusTag
{
	private String key, value;

	public OpusTag(String key, String value)
	{
		this.key = key;
		this.value = value;		
	}

	public String getKey()
	{
		return key;
	}

	public String getValue()
	{
		return value;
	}
}
