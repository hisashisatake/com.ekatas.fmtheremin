/*
 * soundGenerator.cpp
 *
 *  Created on: 2015/04/27
 *      Author: Administrator
 */
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#include "soundGenerator.hpp"

soundGenerator::soundGenerator(playSimpleBufferQueue* q, int n = 5)
{
	owner = q;
	count = n;
	fm = new myFM*[n];

	for (int i = 0; i < count; i++)
	{
		fm[i] = new myFM();
	}

	current = 0;
	next = current + 1;
}

soundGenerator::~soundGenerator()
{
	for (int i = 0; i < count; i++)
	{
		delete fm[i];
	}
	delete [] fm;
}

void soundGenerator::prepareKeyOn()
{
	LOGI("current: %d", current);

	getPriority()->prepareKeyOn();

	current = next++;
	if (next > count)
	{
		current = 0;
		next = current + 1;
	}
}

myFM* soundGenerator::getPriority()
{
	return fm[current];
}

void soundGenerator::clearOutputBuffer()
{
	for (int i = 0; i < OUTPUT_FRAMES; i++)
	{
		outputBuffer[i] = 0;
	}
}

short* soundGenerator::getOutputBuffer()
{
	double num;

	clearOutputBuffer();

	for (int i = 0; i < count; i++)
	{
		if (fm[i]->getKeyOn() == 0)
		{
			continue;
		}
		fm[i]->setTone();
	}

	for (int f = 0; f < OUTPUT_FRAMES; f++)
	{
		num = 0;
		for (int j = 0; j < count; j++)
		{
			num += fm[j]->outputBuffer[f];
		}
		outputBuffer[f] = (num/count) * 10000;
	}

	return outputBuffer;
}
