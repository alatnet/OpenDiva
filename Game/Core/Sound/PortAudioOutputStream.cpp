#include <StdAfx.h>
#include "PortAudioOutputStream.h"

namespace LYGame {
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Constructor/Deconstructor
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	PortAudioOutputStream::PortAudioOutputStream(const char * filename, bool blocking, unsigned int blockingFramesBuffSize, PaDeviceIndex device, void * hostApiSpecificStreamInfo) {
		PaStreamParameters streamParams;
		streamParams.device = device; //set device to use
		streamParams.hostApiSpecificStreamInfo = hostApiSpecificStreamInfo;

		this->init(filename, blocking, blockingFramesBuffSize, streamParams);
	}

	PortAudioOutputStream::~PortAudioOutputStream() {
		this->Stop(); //if we are playing, stop playing.

		if (this->musicStream != NULL) Pa_CloseStream(this->musicStream); //close the stream.
		if (this->sndFile != NULL) sf_close(this->sndFile); //close the libsndfile file.
	}

	void PortAudioOutputStream::init(const char * filename, bool blocking, unsigned int blockingFramesBuffSize, PaStreamParameters params) {
		this->_loop = false;
		this->musicStream = nullptr;
		this->sndFile = nullptr;
		this->position = this->prevPos = 0;
		this->_hasPaError = false;
		this->_hasSfError = false;
		this->_vol = 1.0f;

		this->sndFile = sf_open(filename, SFM_READ, &this->sfInfo);

		if (!this->sndFile) {
			int err = sf_error(this->sndFile);
			pushSfError(err, sf_error_number(err));
			return;
		}

		params.sampleFormat = paFloat32; // 32bit float format
		params.suggestedLatency = 0.2; //200 ms ought to satisfy even the worst sound card
		params.channelCount = this->numChannels = this->sfInfo.channels; // use the same number of channels as our sound file

		/*const PaDeviceInfo* info = Pa_GetDeviceInfo(params.device);
		CryLog("Max Channels: %i", info->maxOutputChannels);*/

		this->blockingFramesPerBuff = blockingFramesBuffSize;

		int err = 0;

		if (!blocking) {
			err = Pa_OpenStream(
				&this->musicStream,
				0, // no input
				&params,
				this->sfInfo.samplerate, // use the same sample rate as the sound file
				paFramesPerBufferUnspecified, // let portaudio choose the buffersize
				paNoFlag,/* no special modes (clip off, dither off) */
				PortAudioOutputStream::paCallbackCommon,
				this
				);
		} else {
			err = Pa_OpenStream(
				&this->musicStream,
				0, // no input
				&params,
				this->sfInfo.samplerate, // use the same sample rate as the sound file
				blockingFramesBuffSize, // let portaudio choose the buffersize
				paNoFlag,/* no special modes (clip off, dither off) */
				nullptr,
				nullptr
				);
		}

		if (err != paNoError) {
			pushPaError(err, Pa_GetErrorText(err));
			this->musicStream = nullptr;
		}

		this->timeLength.totalSec = (1.0 * this->sfInfo.frames) / this->sfInfo.samplerate;
		this->timeLength.hrs = (int)(this->timeLength.totalSec / 3600.0);
		this->timeLength.min = (int)((this->timeLength.totalSec - (this->timeLength.hrs * 3600.0)) / 60.0);
		this->timeLength.sec = this->timeLength.totalSec - (this->timeLength.hrs * 3600.0) - (this->timeLength.min * 60.0);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Basic functionality
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	void PortAudioOutputStream::Play() {
		if (!this->IsPlaying() && this->musicStream != nullptr) {
			int err = Pa_StartStream(this->musicStream);
			if (err != paNoError) pushPaError(err, Pa_GetErrorText(err));
		}
	}

	void PortAudioOutputStream::Pause() {
		if (this->IsPlaying() && this->musicStream != nullptr) {
			int err = Pa_StopStream(this->musicStream);
			if (err != paNoError) pushPaError(err, Pa_GetErrorText(err));
			this->prevPos = this->position;
		}
	}

	void PortAudioOutputStream::Stop() {
		if (this->IsPlaying() && this->musicStream != nullptr) {
			int err = Pa_StopStream(this->musicStream);
			if (err != paNoError) pushPaError(err, Pa_GetErrorText(err));
			this->position = this->prevPos = 0;
		}
	}

	void PortAudioOutputStream::SetVol(float vol) {
		if (vol >= 1.0f) vol = 1.0f;
		if (vol <= 0.0f) vol = 0.0f;
		this->_vol = vol;
	}

	PAOSLength PortAudioOutputStream::GetCurrTimeFormated() {
		PAOSLength ret;

		ret.totalSec = (double)(this->position / (double)(sfInfo.samplerate));
		ret.hrs = (int)(ret.totalSec / 3600.0);
		ret.min = (int)((ret.totalSec - (ret.hrs * 3600.0)) / 60.0);
		ret.sec = ret.totalSec - (ret.hrs * 3600.0) - (ret.min * 60.0);

		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Blocking IO functions
	//May not use...
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	void PortAudioOutputStream::ReadIO() {
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Error checking
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	PAOSError PortAudioOutputStream::GetPaError() {
		if (this->paErrors.size() == 0) {
			PAOSError noErr;
			return noErr;
		}

		PAOSError ret = this->paErrors.at(this->paErrors.size() - 1);
		this->paErrors.pop_back();

		if (this->paErrors.size() == 0) this->_hasPaError = false;

		return ret;
	}

	PAOSError PortAudioOutputStream::GetSfError() {
		if (this->sfErrors.size() == 0) {
			PAOSError noErr;
			return noErr;
		}

		PAOSError ret = this->sfErrors.at(this->sfErrors.size() - 1);
		this->sfErrors.pop_back();

		if (this->sfErrors.size() == 0) this->_hasSfError = false;

		return ret;
	}

	void PortAudioOutputStream::pushPaError(int errorCode, const char * errorStr) {
		PAOSError err;
		err.code = errorCode;
		err.str = errorStr;
		this->paErrors.push_back(err);
		this->_hasPaError = true;
	}

	void PortAudioOutputStream::pushSfError(int errorCode, const char * errorStr) {
		PAOSError err;
		err.code = errorCode;
		err.str = errorStr;
		this->sfErrors.push_back(err);
		this->_hasSfError = true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//THE ALL MIGHTY CALLBACKS!
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	int PortAudioOutputStream::paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
		this->prevPos = this->position;
		bool _EOF = false;

		AudioFrame::_2 *outFrame = (AudioFrame::_2*)outputBuffer;

		sf_seek(this->sndFile, this->position, SF_SEEK_SET);

		AudioFrame::_2 * frames = new AudioFrame::_2[framesPerBuffer];
		long framesRead = (long)sf_readf_float(this->sndFile, (float *)frames, framesPerBuffer);

		if (framesRead + this->position >= this->sfInfo.frames) _EOF = true;

		for (unsigned long i = 0; i < framesRead; i++) {
			outFrame[i].left = frames[i].left * this->_vol;
			outFrame[i].right = frames[i].right * this->_vol;
		}
		delete[] frames;

		this->position += framesRead;

		/*for (unsigned long i = 0; i < framesPerBuffer; i++) {
			if (i > (this->sfInfo.frames - this->position)) {
				this->position = this->prevPos = 0;
				_EOF = true;
				break;
			}

			AudioFrame::_2 frame;
			float *buff = (float *)(&frame);
			sf_readf_float(this->sndFile, buff, 1);

			outFrame->left = frame.left * this->_vol;
			outFrame->right = frame.right * this->_vol;
			*outFrame++;
			this->position++;
		}*/

		if (_EOF && !this->_loop) { //if we are at the end of the file and dont want to loop.
			this->position = this->prevPos = 0;
			return paComplete; //we are done with playback
		}

		return paContinue; //continue playback
	}

	int PortAudioOutputStream::paCallbackCommon(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
		PortAudioOutputStream * data = (PortAudioOutputStream *)userData;
		//redirect to the individual callback. (gives us individualized and possibly multiple streams at the same time).
		return data->paCallback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags, userData);
	}
}