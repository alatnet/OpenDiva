#ifndef _H_PORTAUDIOOUTPUTSTREAM_
#define _H_PORTAUDIOOUTPUTSTREAM_
#pragma once
#include <sndfile.hpp>

#include "Source/IAudioSource.h"

namespace LYGame {
	struct PAOSError {
		int code;
		const char * str;

		PAOSError() :
			code(0),
			str("NoError") {
		};
	};

	struct PAOSLength {
		int hrs, min;
		double sec, totalSec;

		PAOSLength() :
			hrs(0),
			min(0),
			sec(0.0f),
			totalSec(0.0f)
		{};
	};

	//because /WX is fucking with us...
	//#ifdef WX_DISABLE
	//	#pragma warning(disable : 4244)
	//#endif

	WX_DISABLE_(4244)

	class PortAudioOutputStream {
	public:
		PortAudioOutputStream(const char * filename, bool blocking = false, unsigned int blockingFramesBuffSize = 1024, PaDeviceIndex device = Pa_GetDefaultOutputDevice(), void * hostApiSpecificStreamInfo = 0);
		~PortAudioOutputStream();
	private:
		void init(const char * filename, bool blocking, unsigned int blockingFramesBuffSize, PaStreamParameters params);
	public: //basic functionality
		void Play();
		void Pause();
		void Stop();

		void SetVol(float vol);
		void SetLoop(bool loop) { this->_loop = loop; }

		bool IsPlaying() { return (this->musicStream != nullptr) ? (Pa_IsStreamActive(this->musicStream) == 1) : false; }
		bool IsLooping() { return this->_loop; }

		double GetCurrTime() { return (double)(this->position / (double)(this->sfInfo.samplerate)); } //returns seconds
		double GetDeltaTime() { return (this->position > this->prevPos) ? (double)((this->position - this->prevPos) / (double)(this->sfInfo.samplerate)) : 0; }
		PAOSLength GetCurrTimeFormated();
		PaTime GetPaTime() { return Pa_GetStreamTime(this->musicStream); } //returns seconds
		PAOSLength GetLength() { return this->timeLength; }

		void SetPosition(double seconds) { this->position = seconds * (double)(this->sfInfo.samplerate); }
	public: //blocking io functions
		void ReadIO();
	public: //error checking
		bool HasError() { return HasPaError() || HasSfError(); }
		//Port Audio
		bool HasPaError() { return _hasPaError; }
		//libsndfile
		bool HasSfError() { return _hasSfError; }

		//Port Audio
		PAOSError GetPaError();
		//libsndfile
		PAOSError GetSfError();
	private: //error stuff
		bool _hasPaError, _hasSfError;
		std::vector<PAOSError> paErrors;
		std::vector<PAOSError> sfErrors;
		void pushPaError(int errorCode, const char * errorStr);
		void pushSfError(int errorCode, const char * errorStr);
	private: //port audio stuff
		PaStream *musicStream;
		PAOSLength timeLength;
	private: //blocking port audio stuff
		unsigned int blockingFramesPerBuff;
		float *blockingBuff;
	private: //other stuff
		bool _loop;
		float _vol;
		unsigned int numChannels;
	private: //callback variables.
		SNDFILE *sndFile;
		SF_INFO sfInfo;
		long long position, prevPos;
	protected:
		int paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
	public:
		static int paCallbackCommon(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
	};

	//#ifdef WX_DISABLE
	//	#pragma warning(default : 4244)
	//#endif

	WX_ENABLE_(4244)
}

#endif //_H_PORTAUDIOOUTPUTSTREAM_