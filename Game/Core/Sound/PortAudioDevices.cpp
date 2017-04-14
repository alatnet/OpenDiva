#include <StdAfx.h>
#include "PortAudioDevices.h"

NSGameStart
	PortAudioDevices::PortAudioDevices() {
	}

	PortAudioDevices::~PortAudioDevices() {
	}

	PADError PortAudioDevices::GetError() {
		if (this->errors.size() == 0) {
			PADError noErr;
			return noErr;
		}

		PADError ret = this->errors.at(this->errors.size() - 1);
		this->errors.pop_back();

		if (this->errors.size() == 0) this->_hasError = false;

		return ret;
	}

	void PortAudioDevices::pushPaError(int errorCode, const char * errorStr) {
		PADError err;
		err.code = errorCode;
		err.str = errorStr;
		this->errors.push_back(err);
		this->_hasError = true;
	}
NSGameEnd