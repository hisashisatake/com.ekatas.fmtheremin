/*
 * soundGenerator.hpp
 *
 *  Created on: 2015/04/25
 *      Author: Administrator
 */

#ifndef SOUNDGENERATOR_HPP_
#define SOUNDGENERATOR_HPP_

#include <pthread.h>

#include "myFM.hpp"
#include "playSimpleBufferQueue.hpp"

class playSimpleBufferQueue;

class soundGenerator
{
private:
	int count;
	int current;
	int next;
	myFM** fm;
	short outputBuffer[OUTPUT_FRAMES];
	void clearOutputBuffer();
	pthread_mutex_t mutex;
	playSimpleBufferQueue* owner;

public:
	soundGenerator(playSimpleBufferQueue* q, int n);
	virtual ~soundGenerator();
	virtual myFM* getPriority();
	virtual short* getOutputBuffer();
	virtual void prepareKeyOn();
};

#endif /* SOUNDGENERATOR_HPP_ */
