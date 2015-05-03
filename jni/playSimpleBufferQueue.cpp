/*
 * BufferQueue.cpp
 *
 *  Created on: 2015/05/02
 *      Author: Administrator
 */

#include "playSimpleBufferQueue.hpp"

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

// Constructor
playSimpleBufferQueue::playSimpleBufferQueue()
{
	bqPlayerObject = NULL;
	engineObject = NULL;
	outputMixObject = NULL;
	bqPlayerBufferQueue = NULL;
	engineEngine = NULL;
	bqPlayerPlay = NULL;

	pthread_mutex_init(&mutex, NULL);

	generateSound = new soundGenerator(this, 5);

	//createThreadLock();
	initialize();
}

	// Destructor
playSimpleBufferQueue::~playSimpleBufferQueue()
{
	LOGI("BufferQueue destructor");

	terminate();

	//notifyThreadLock();
	//destroyThreadLock();

	delete generateSound;

	pthread_mutex_destroy(&mutex);
}

// this callback handler is called every time a buffer finishes playing
void playSimpleBufferQueue::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void* context)
{
	if (bq == NULL || context == NULL)
	{
		LOGI("context is already destroyed");
		return;
	}

	playSimpleBufferQueue* q = (playSimpleBufferQueue*)context;

	pthread_mutex_lock(&q->mutex);

	assert(bq == q->bqPlayerBufferQueue);

	short* buffer = q->generateSound->getOutputBuffer();
	SLresult result = (*q->bqPlayerBufferQueue)->Enqueue(q->bqPlayerBufferQueue,
														 (void*)buffer,
														 sizeof(*buffer) * OUTPUT_FRAMES);
	if (SL_RESULT_SUCCESS != result)
	{
		LOGI("SL_RESULT_SUCCESS: %d", result);
	}
	//assert(SL_RESULT_SUCCESS == result);

	//LOGI("call bqPlayerCallback");
	//q->notifyThreadLock();

	pthread_mutex_unlock(&q->mutex);
}

void playSimpleBufferQueue::initialize()
{
	//soundGenerator->setFreq(0);
	//soundGenerator->setAmp(0);
	createEngine();
	createBufferQueueAudioPlayer();
}

void playSimpleBufferQueue::terminate()
{
	LOGI("BufferQueue terminate");

	LOGI("BufferQueue setStop");
	setStop();

	LOGI("BufferQueue shutdown");
	shutdown();
}

void playSimpleBufferQueue::prepareKeyOn()
{
	generateSound->prepareKeyOn();
}

void playSimpleBufferQueue::setFreq(double freq)
{
//		pthread_mutex_lock(&mutex);
	LOGI("Freq:%f", freq);
	myFM* sg = generateSound->getPriority();
	sg->setFreq(freq);
//	    pthread_mutex_unlock(&mutex);
}

void playSimpleBufferQueue::setAmp(double amp)
{
//		pthread_mutex_lock(&mutex);
	myFM* sg = generateSound->getPriority();
	sg->setAmp(amp);
//	    pthread_mutex_unlock(&mutex);
}

int playSimpleBufferQueue::getKeyOn()
{
//		pthread_mutex_lock(&mutex);
	myFM* sg = generateSound->getPriority();
	return sg->keyon;
//	    pthread_mutex_unlock(&mutex);
}

void playSimpleBufferQueue::enqueue()
{
//		pthread_mutex_lock(&mutex);
	short* buffer = generateSound->getOutputBuffer();
	SLresult result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
													  (void*)buffer,
													  sizeof(*buffer) * OUTPUT_FRAMES);
	LOGI("SL_RESULT_SUCCESS: %d", result);
	//assert(SL_RESULT_SUCCESS == result);
	//waitThreadLock();
//	    pthread_mutex_unlock(&mutex);
}

void playSimpleBufferQueue::setStop()
{
//		pthread_mutex_lock(&mutex);
	SLresult result;

	LOGI("setStop: enqueue empty buffer");
	//myFM* sg = generateSound->getPriority();

	//sg->nextBuffer = NULL;
	//sg->nextSize = 0;
	//result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, sg->nextBuffer, sg->nextSize);
	//assert(SL_RESULT_SUCCESS == result);

	LOGI("setStop: clear player buffer queue");
	result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);

	LOGI("setStop: state change to PAUSED");
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	assert(SL_RESULT_SUCCESS == result);

//	    pthread_mutex_unlock(&mutex);
}

void playSimpleBufferQueue::shutdown()
{
	// destroy buffer queue audio player object, and invalidate all associated interfaces
	if (bqPlayerObject != NULL) {
		(*bqPlayerObject)->Destroy(bqPlayerObject);
		bqPlayerObject = NULL;
		bqPlayerPlay = NULL;
		bqPlayerBufferQueue = NULL;
	}
	// destroy output mix object, and invalidate all associated interfaces
	if (outputMixObject != NULL) {
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (engineObject != NULL) {
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
}

void playSimpleBufferQueue::createEngine()
{
	SLresult result;

	// create engine
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result);

	// realize the engine
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	// get the engine interface, which is needed in order to create other objects
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	assert(SL_RESULT_SUCCESS == result);

	// create output mix, with environmental reverb specified as a non-required interface
	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
	assert(SL_RESULT_SUCCESS == result);

	// realize the output mix
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
}

// create buffer queue audio player
void playSimpleBufferQueue::createBufferQueueAudioPlayer()
{
	SLresult result;

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
													   2};
	SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
								   1,
								   SL_SAMPLINGRATE_44_1,
								   SL_PCMSAMPLEFORMAT_FIXED_16,
								   SL_PCMSAMPLEFORMAT_FIXED_16,
								   SL_SPEAKER_FRONT_CENTER,
								   SL_BYTEORDER_LITTLEENDIAN};
	SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE,
								  SL_IID_EFFECTSEND};
	const SLboolean req[2]     = {SL_BOOLEAN_TRUE,
								  SL_BOOLEAN_TRUE};
	result = (*engineEngine)->CreateAudioPlayer(engineEngine,
												&bqPlayerObject,
												&audioSrc,
												&audioSnk,
												2,
												ids,
												req);
	assert(SL_RESULT_SUCCESS == result);

	// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject,
										SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject,
											 SL_IID_PLAY,
											 &bqPlayerPlay);
	assert(SL_RESULT_SUCCESS == result);

	// get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject,
											 SL_IID_BUFFERQUEUE,
											 &bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);

	// register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,
													  // callback static method
													  &playSimpleBufferQueue::bqPlayerCallback,
													  // context is "this"
													  this);
	assert(SL_RESULT_SUCCESS == result);

	// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
}



