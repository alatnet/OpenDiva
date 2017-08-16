#ifndef _H_PORTAUDIODEVICES_
#define _H_PORTAUDIODEVICES_
#pragma once

#include "Port Audio System\PortAudioSystemCommon.h"

//port audio system
#include <portaudio.h>
#ifdef PAS_WIN
	#include <pa_win_wmme.h>
	#include <pa_win_ds.h>
	#include <pa_win_wdmks.h>
	#include <pa_asio.h>
	#include <pa_win_wasapi.h>
	#include <pa_win_waveformat.h>
#elif PAS_MAC
	#include <pa_mac_core.h>
	#include <pa_jack.h>
#elif PAS_NIX
	#include <pa_jack.h>
	#include <pa_linux_alsa.h>
#endif

NSGameStart
	struct PADError {
		int code;
		const char * str;

		PADError() :
			code(0),
			str("NoError") {
		};
	};

	struct PADInfo {
		const char * device;
		PaDeviceIndex index;
		PaHostApiIndex hostIndex;
	};

	class PortAudioDevices {
	public:
		PortAudioDevices();
		~PortAudioDevices();
	private:
		std::vector<PADInfo> devices;
	public: //error checking
		bool HasError() { return _hasError; }
		PADError GetError();
	private:
		bool _hasError;
		std::vector<PADError> errors;
		void pushPaError(int errorCode, const char * errorStr);
	};
NSGameEnd

#endif //_H_PORTAUDIODEVICES_