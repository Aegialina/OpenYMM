/**
 * \file
 * \brief Source code for the Frame class
 * \author Copyright (c) 2011 Jonathan Thomas
 */

#include "../include/Frame.h"

using namespace std;
using namespace openshot;

// Constructor - blank frame (300x200 blank image, 48kHz audio silence)
Frame::Frame() : number(1), image(0), audio(0)
{
	// Init the image magic and audio buffer
	image = new Magick::Image(Magick::Geometry(300,200), Magick::Color("red"));
	audio = new juce::AudioSampleBuffer(2,1600);

	// initialize the audio samples to zero (silence)
	audio->clear();
};

// Constructor - image only (48kHz audio silence)
Frame::Frame(int number, int width, int height, string color) : number(number), image(0), audio(0)
{
	// Init the image magic and audio buffer
	image = new Magick::Image(Magick::Geometry(width, height), Magick::Color(color));
	audio = new juce::AudioSampleBuffer(2,1600);

	// initialize the audio samples to zero (silence)
	audio->clear();
};

// Constructor - image only from pixel array (48kHz audio silence)
Frame::Frame(int number, int width, int height, const string map, const Magick::StorageType type, const void *pixels)
	: number(number), image(0), audio(0)
{
	// Init the image magic and audio buffer
	image = new Magick::Image(width, height, map, type, pixels);
	audio = new juce::AudioSampleBuffer(2,1600);

	// initialize the audio samples to zero (silence)
	audio->clear();
};

// Constructor - audio only (300x200 blank image)
Frame::Frame(int number, int samples, int channels) : number(number), image(0), audio(0)
{
	// Init the image magic and audio buffer
	image = new Magick::Image(Magick::Geometry(300, 200), Magick::Color("white"));
	audio = new juce::AudioSampleBuffer(channels, samples);

	// initialize the audio samples to zero (silence)
	audio->clear();
};

// Constructor - image & audio
Frame::Frame(int number, int width, int height, string color, int samples, int channels) : number(number), image(0), audio(0)
{
	// Init the image magic and audio buffer
	image = new Magick::Image(Magick::Geometry(width, height), Magick::Color(color));
	audio = new juce::AudioSampleBuffer(channels, samples);

	// initialize the audio samples to zero (silence)
	audio->clear();
};

// Destructor
Frame::~Frame()
{
	// deallocate image and audio memory
	DeletePointers();
}

// Copy constructor
Frame::Frame ( const Frame &other )
{
	// copy pointers and data
	DeepCopy(other);
}

// Assignment operator
Frame& Frame::operator= (const Frame& other)
{
	if (this != &other) {
		// deallocate image and audio memory
		DeletePointers();

		// copy pointers and data
		DeepCopy(other);
	}

	// return this instance
	return *this;
  }

// Copy data and pointers from another Frame instance
void Frame::DeepCopy(const Frame& other)
{
	// ignore copy if objects are the same
	number = other.number;
	image = new Magick::Image(*(other.image));
	audio = new juce::AudioSampleBuffer(*(other.audio));
}

// Deallocate image and audio memory
void Frame::DeletePointers()
{
	// deallocate image memory
	delete image;
	image = NULL;
	// deallocate audio memory
	delete audio;
	audio = NULL;
}

// Display the frame image to the screen (primarily used for debugging reasons)
void Frame::Display()
{
	// display the image (if any)
	if (image->size().width() > 1 && image->size().height() > 1)
		image->display();
}

// Display the wave form
void Frame::DisplayWaveform(bool resize)
{
	// Create blank image
	Magick::Image wave_image;

	// Init a list of lines
	list<Magick::Drawable> lines;
	lines.push_back(Magick::DrawableFillColor("#0070ff"));
	lines.push_back(Magick::DrawablePointSize(16));

	// Calculate the width of an image based on the # of samples
	int width = audio->getNumSamples();

	if (width > 0)
	{
		// If samples are present...
		int height = 200 * audio->getNumChannels();
		int height_padding = 20 * (audio->getNumChannels() - 1);
		int total_height = height + height_padding;
		wave_image = Magick::Image(Magick::Geometry(width, total_height), Magick::Color("#000000"));

		// Loop through each audio channel
		int Y = 100;
		for (int channel = 0; channel < audio->getNumChannels(); channel++)
		{
			// Get audio for this channel
			float *samples = audio->getSampleData(channel);

			for (int sample = 0; sample < audio->getNumSamples(); sample++)
			{
				// Sample value (scaled to -100 to 100)
				float value = samples[sample] * 100;

				if (value > 100 || value < -100)
					cout << "TOO BIG!!! " << value << endl;

				// Append a line segment for each sample
				if (value != 0.0)
				{
					// LINE
					lines.push_back(Magick::DrawableStrokeColor("#0070ff"));
					lines.push_back(Magick::DrawableStrokeWidth(1));
					lines.push_back(Magick::DrawableLine(sample,Y, sample,Y-value)); // sample=X coordinate, Y=100 is the middle
				}
				else
				{
					// DOT
					lines.push_back(Magick::DrawableFillColor("#0070ff"));
					lines.push_back(Magick::DrawableStrokeWidth(1));
					lines.push_back(Magick::DrawablePoint(sample,Y));
				}
			}

			// Add Channel Label
			stringstream label;
			label << "Channel " << channel;
			lines.push_back(Magick::DrawableStrokeColor("#ffffff"));
			lines.push_back(Magick::DrawableFillColor("#ffffff"));
			lines.push_back(Magick::DrawableStrokeWidth(0.1));
			lines.push_back(Magick::DrawableText(5, Y - 5, label.str()));

			// Increment Y
			Y += (200 + height_padding);
		}

		// Draw the waveform
		wave_image.draw(lines);

		// Resize Image (if requested)
		if (resize)
			// Resize to 60%
			wave_image.resize(Magick::Geometry(width * 0.6, total_height * 0.6));
	}
	else
	{
		// No audio samples present
		wave_image = Magick::Image(Magick::Geometry(720, 480), Magick::Color("#000000"));

		// Add Channel Label
		lines.push_back(Magick::DrawableStrokeColor("#ffffff"));
		lines.push_back(Magick::DrawableFillColor("#ffffff"));
		lines.push_back(Magick::DrawableStrokeWidth(0.1));
		lines.push_back(Magick::DrawableText(265, 240, "No Audio Samples Found"));

		// Draw the waveform
		wave_image.draw(lines);
	}

	// Display Image
	wave_image.display();
}

// Get pixel data (as packets)
const Magick::PixelPacket* Frame::GetPixels()
{
	// Return arry of pixel packets
	return image->getConstPixels(0,0, image->columns(), image->rows());
}

// Get pixel data (for only a single scan-line)
const Magick::PixelPacket* Frame::GetPixels(int row)
{
	// Return arry of pixel packets
	return image->getConstPixels(0,row, image->columns(), 1);
}

// Get pixel data (for a resized image)
const Magick::PixelPacket* Frame::GetPixels(unsigned int width, unsigned int height, int frame)
{
	// Create a new resized image
	//Magick::Image newImage = *image;
	small_image = new Magick::Image(*(image));
	small_image->resize(Magick::Geometry(width, height));
	small_image->colorize(255, 0, 0, Magick::Color(0,0,255));
	small_image->blur(5.0, 5.0);

	stringstream file;
	file << "frame" << frame << ".png";
	small_image->write(file.str());

	// Return arry of pixel packets
	return small_image->getConstPixels(0,0, small_image->columns(), small_image->rows());
}

// Get height of image
int Frame::GetHeight()
{
	// return height
	return image->rows();
}

// Get height of image
int Frame::GetWidth()
{
	// return width
	return image->columns();
}

// Save the frame image
void Frame::Save()
{
	// save the image
	stringstream file;
	file << "frame" << number << ".png";
	image->write(file.str());
}

// Add (or replace) pixel data to the frame
void Frame::AddImage(int width, int height, const string map, const Magick::StorageType type, const void *pixels)
{
	// Deallocate image memory
	delete image;
	image = NULL;

	// Create new image object, and fill with pixel data
	image = new Magick::Image(width, height, map, type, pixels);
}

// Add audio samples to a specific channel
void Frame::AddAudio(int destChannel, int destStartSample, const float* source, int numSamples, float gainToApplyToSource = 1.0f)
{
	// Add samples to frame's audio buffer
	audio->addFrom(destChannel, destStartSample, source, numSamples, gainToApplyToSource);
}

// Play audio samples for this frame
void Frame::Play()
{
	// Check if samples are present
	if (!audio->getNumSamples())
		return;

	AudioDeviceManager deviceManager;
	deviceManager.initialise (0, /* number of input channels */
	        audio->getNumChannels(), /* number of output channels */
	        0, /* no XML settings.. */
	        true  /* select default device on failure */);

	AudioFormatManager formatManager;
	formatManager.registerBasicFormats();

	AudioSourcePlayer audioSourcePlayer;
	deviceManager.addAudioCallback (&audioSourcePlayer);

	ScopedPointer<AudioBufferSource> my_source;
	my_source = new AudioBufferSource(audio->getNumSamples(), audio->getNumChannels());

	// Add audio to AudioBufferSource
	for (int channel = 0; channel < audio->getNumChannels(); channel++)
	{
		// Add audio for each channel
		my_source->AddAudio(channel, 0, audio->getSampleData(channel), audio->getNumSamples(), 1.0f);
	}

	AudioTransportSource transport1;
	transport1.setSource (my_source,
			5000, // tells it to buffer this many samples ahead
			(double) 48000); // sample rate of source
	transport1.setPosition (0);
	transport1.setGain(1.0);

	// Create MIXER
	MixerAudioSource mixer;
	mixer.addInputSource(&transport1, false);
	audioSourcePlayer.setSource (&mixer);

	// Start transports
	transport1.start();

	while (transport1.isPlaying())
	{
		cout << "playing" << endl;
		sleep(1);
	}

	cout << "DONE!!!" << endl;

	transport1.stop();
    transport1.setSource (0);
    audioSourcePlayer.setSource (0);
    deviceManager.removeAudioCallback (&audioSourcePlayer);
    deviceManager.closeAudioDevice();
    deviceManager.removeAllChangeListeners();
    deviceManager.dispatchPendingMessages();

	cout << "End of Play()" << endl;


}




