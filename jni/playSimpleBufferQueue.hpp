/*
 * playSimpleBufferQueue.hpp
 *
 *  Created on: 2015/04/05
 *      Author: hisashisatake
 */

#ifndef PLAYSIMPLEBUFFERQUEUE_HPP_
#define PLAYSIMPLEBUFFERQUEUE_HPP_

#include <EGL/egl.h>
#include <GLES/gl.h>
// for native audio
#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"

#include "fm.hpp"

// イベントリスナインターフェース
class listenerBase {
public:
    // イベント
    virtual void invoke(SLAndroidSimpleBufferQueueItf caller, void *pContext) = 0;
    virtual ~listenerBase() = 0;
};

// メソッドのポインタをイベントリスナとするためのテンプレートクラス
template <class T>
class eventListener : listenerBase {
public:
    // コンストラクタ
    eventListener(T* obj, void (T::*callback)(SLAndroidSimpleBufferQueueItf, void *)){
        this->obj = obj;
        this->callback = callback;
    };

    // イベント
    virtual void invoke(SLAndroidSimpleBufferQueueItf caller, void *pContext){
        (obj->*callback)(caller, pContext);
    };
private:
    T* obj;
    void (T::*callback)(SLAndroidSimpleBufferQueueItf, void *);
};

// イベント送信側
class eventSender{
public:
    // コールバックメソッドを登録
    template <class T>
    void setListener(T* obj, void (T::*callback)(SLAndroidSimpleBufferQueueItf, void *)){
        listener = new eventListener<T>(obj, callback);
    };

    // イベントを発生させる
    void Fire(SLAndroidSimpleBufferQueueItf caller, void *pContext){
        listener->invoke(caller, pContext);
    }
private:
    listenerBase* listener;
};

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
	static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

public:
	static myFM* soundGenerator;

	playSimpleBufferQueue()
	{
		bqPlayerObject = NULL;
		engineObject = NULL;
		outputMixObject = NULL;
		bqPlayerBufferQueue = NULL;
	}

	void setSoundGenerator()
	{

	}

	// this callback handler is called every time a buffer finishes playing
	static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
	{
	    assert(bq == bqPlayerBufferQueue);
	    assert(NULL == context);

	    soundGenerator->setTone();

	    SLresult result;
	    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, soundGenerator->nextBuffer, soundGenerator->nextSize);
	    assert(SL_RESULT_SUCCESS == result);

	    //LOGI("call bqPlayerCallback");
	}

	void setStop()
	{
	    SLresult result;
	    soundGenerator->nextBuffer = NULL;
	    soundGenerator->nextSize = 0;
	    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, soundGenerator->nextBuffer, soundGenerator->nextSize);
	    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	    assert(SL_RESULT_SUCCESS == result);
	}

	void shutdown()
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
public:
	void createEngine()
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
	void createBufferQueueAudioPlayer()
	{
	    SLresult result;

	    // configure audio source
	    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
	    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_44_1,
	        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
	        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
	    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	    // configure audio sink
	    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	    SLDataSink audioSnk = {&loc_outmix, NULL};

	    // create audio player
	    const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND};
	    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
	            2, ids, req);
	    assert(SL_RESULT_SUCCESS == result);

	    // realize the player
	    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	    assert(SL_RESULT_SUCCESS == result);

	    // get the play interface
	    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	    assert(SL_RESULT_SUCCESS == result);

	    // get the buffer queue interface
	    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
	            &bqPlayerBufferQueue);
	    assert(SL_RESULT_SUCCESS == result);

	    // register callback on the buffer queue
	    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, &bqPlayerCallback, NULL);
	    assert(SL_RESULT_SUCCESS == result);

	    // set the player's state to playing
	    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	    assert(SL_RESULT_SUCCESS == result);
	}
};

#endif /* PLAYSIMPLEBUFFERQUEUE_HPP_ */
