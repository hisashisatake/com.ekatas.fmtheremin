/*
 * playSimpleplaySimpleBufferQueue.hpp
 *
 *  Created on: 2015/04/05
 *      Author: hisashisatake
 */

#ifndef PLAYSIMPLEBUFFERQUEUE_HPP_
#define PLAYSIMPLEBUFFERQUEUE_HPP_

#include <unistd.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
// for native audio
#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"
#include <pthread.h>

//#include "threadLocker.hpp"
#include "soundGenerator.hpp"

class soundGenerator;

class playSimpleBufferQueue {
private:
	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;

	// output mix interfaces
	SLObjectItf outputMixObject;

	// buffer queue player interfaces
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

	soundGenerator* generateSound;

public:
	pthread_mutex_t mutex;

	playSimpleBufferQueue();
	// Destructor
	virtual ~playSimpleBufferQueue();

	// this callback handler is called every time a buffer finishes playing
	static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void* context);

	virtual void initialize();
	virtual void terminate();
	virtual void prepareKeyOn();
	virtual void setFreq(double freq);
	virtual void setAmp(double amp);
	virtual int getKeyOn();
	virtual void enqueue();
	void setStop();
	void shutdown();
	void createEngine();
	// create buffer queue audio player
	void createBufferQueueAudioPlayer();
};

#endif /* PLAYSIMPLEBUFFERQUEUE_HPP_ */
