/**\file			audio.h
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Sunday, February 21, 2016
 * \brief			Abstraction to SDL_mixer interface.
 * \details
 * This files is responsible for overall Audio system configuration.  To play
 * music or sound see the corresponding Music and Sound classes.
 */

#ifndef __H_AUDIO__
#define __H_AUDIO__

#include "includes.h"

/** Maximum audio volume */
#define AUDIO_MAX_VOL 128

class Audio {
	public:
		static Audio *Instance();

		bool Initialize( void );
		bool Shutdown( void );

		bool SetMusicVol ( float volume );
		bool SetSoundVol ( float volume );
		float GetMusicVol () { return music_vol; }
		float GetSoundVol () { return sound_vol; }
		int GetFreeChannel( void );
		int GetTotalChannels( void );

		int PlayChannel( int chan, Mix_Chunk *chunk, int loop );

	protected:
		Audio();
		Audio( const Audio & );
  		Audio& operator= (const Audio&);

	private:
		static Audio *pInstance;

		~Audio();

		bool initialized;	// Initialization status
		int audio_rate;		// Samplerate
		Uint16 audio_format;	// AUDIO_S16
		int audio_channels;	// Stereo, etc.
		int audio_buffers;	// Size of buffer
		float music_vol;	// Sound volumes
		float sound_vol;	// Sound volumes
		unsigned int max_chan;	// Total number of channels request
		list<int> lastplayed;	// Queue of last used channels.
};

#endif // __H_AUDIO__
