/*
 * fm.h
 *
 *  Created on: 2013/03/13
 *      Author: hisashisatake
 */

#ifndef FM_H_
#define FM_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef struct
{
  int fs; /* 標本化周波数 */
  int bits; /* 量子化精度 */
  int length; /* 音データの長さ */
  double *s; /* 音データ */
} MONO_PCM;

#define OUTPUT_FRAMES 10
#define PI 3.141592

class myFM
{
public:
	short outputBuffer[OUTPUT_FRAMES];
	short *nextBuffer;
	unsigned nextSize;
	int nextCount;
	int keyon;
	int fmCount;
	double freqNum;
	double ampNum;
	//static double phase;

	myFM()
	{
		keyon = 0;
		freqNum = 0.0;
		ampNum = 0.0;
		fmCount = 0;
		nextBuffer = NULL;
		nextSize = 0;
		nextCount = 0;
	}

	void gen_fm(double fc)
	{
		MONO_PCM pcm;
		int n;
		int A, D, R, gate, duration;
		//    double *ac, fc, *am, ratio, gain, S;
		double S, ac, am, num, fm;

		pcm.fs = 44100; /* 標本化周波数 */
		pcm.bits = 16; /* 量子化精度 */
		pcm.length = OUTPUT_FRAMES; /* 音データの長さ */
		//pcm.s = (double*)calloc(pcm.length, sizeof(double)); /* 音データ */

		//ac = (double*)calloc(pcm.length, sizeof(double));
		//am = (double*)calloc(pcm.length, sizeof(double));

#if 0
		/* キャリア振幅 */
		gate = pcm.fs * 4;
		duration = pcm.fs * 4;
		A = 0;
		D = pcm.fs * 4;
		S = 0.0;
		R = pcm.fs * 4;
		ADSR(ac, A, D, S, R, gate, duration);

		fc = 440.0; /* キャリア周波数 */

		/* モジュレータ振幅 */
		gate = pcm.fs * 4;
		duration = pcm.fs * 4;
		A = 0;
		D = pcm.fs * 2;
		S = 0.0;
		R = pcm.fs * 2;
		ADSR(am, A, D, S, R, gate, duration);

		ratio = 3.5;
		fm = fc * ratio; /* モジュレータ周波数 */
#endif

		A = 0;
		D = pcm.fs * 4;
		S = 0.0;
		R = pcm.fs * 4;
		gate = pcm.fs * 4;
		duration = pcm.fs * 4;
		ac = ADSR(A, D, S, R, gate, duration);

		gate = pcm.fs * 4;
		duration = pcm.fs * 4;
		A = 0;
		D = pcm.fs * 8;
		S = 0.0;
		R = pcm.fs * 2;
		am = ADSR(A, D, S, R, gate, duration);

		fm = fc * 3.5;

		/* FM音源 */
	//    for (n = 0; n < pcm.length; n++)
		for (n = 0; n < OUTPUT_FRAMES; ++n)
		{
			num = ac * sin(2.0 * M_PI * fc * keyon / pcm.fs + am * sin(2.0 * M_PI * fm * keyon / pcm.fs));
			//num = sin(2.0 * M_PI * fc * keyon / pcm.fs * sin(2.0 * M_PI * fm * keyon / pcm.fs));
			//num = sin(2.0 * M_PI * fc * keyon / pcm.fs);
			outputBuffer[n] = num * 10000;

			//ログを出力
			//LOGI("buf:%f", outputBuffer[n]);
			//if (keyon % 1000 == 0) {
			//	LOGI("keyon:%d", keyon);
			//}

			keyon++;
		}

		//free(pcm.s);
		//free(ac);
		//free(am);
	}

	void setFreq(double count)
	{
	    freqNum = count;
	}

	void setAmp(double count)
	{
	    ampNum = count;
	}

	void setTone()
	{
#if 0
		unsigned i;
		for (i = 0; i < OUTPUT_FRAMES; ++i) {
			phase += 22050*freqNum / 44100.0;
			phase = (phase >= 1.0) ? -1 : phase;
			outputBuffer[i] = (phase < 0) ? 0 : 32768*ampNum;
		}
#endif
		gen_fm(freqNum * 10000);
		nextBuffer = outputBuffer;
		nextSize = sizeof(outputBuffer);
	}
#if 0
	void setTone()
	{
		//SLresult result;
		tone_dynamic2();
		nextBuffer = outputBuffer;
		nextSize = sizeof(outputBuffer);
		//result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
		//LOGI("call setTone");
	}
#endif
	double ADSR(int A, int D, double S, int R, int gate, int duration)
	{
		if (A != 0)
		{
			if (keyon < A) {
				return 1.0 - exp(-5.0 * keyon / A);
			}
		}

		if (D != 0)
		{
			if (keyon >= A && (keyon < D && keyon < gate)) {
				return S + (1 - S) * exp(-5.0 * (keyon - A) / D);
			}
		}
		else
		{
			if (keyon >= A && (keyon < D && keyon < gate)) {
				return S;
			}
		}

		return S;

#if 0
		if (R != 0) {
			if (keyon >=A && keyon >=D && keyon < gate) {
				return e[gate - 1] * exp(-5.0 * (n - gate + 1) / R);
			}
		}
#endif
	}
};
#endif /* FM_H_ */
