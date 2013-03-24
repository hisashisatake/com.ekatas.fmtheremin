/*
 * fm.h
 *
 *  Created on: 2013/03/13
 *      Author: satake
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
//  int length; /* 音データの長さ */
//  double *s; /* 音データ */
} MONO_PCM;

#define OUTPUT_FRAMES 100 * 10
static short outputBuffer[OUTPUT_FRAMES];

static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

static unsigned int keyon = 0;
static unsigned int fmCount;

void ADSR(double e[], int A, int D, double S, int R, int gate, int duration)
{
  int n;

  if (A != 0)
  {
    for (n = 0; n < A; n++)
    {
      e[n] = 1.0 - exp(-5.0 * n / A);
    }
  }

  if (D != 0)
  {
    for (n = A; n < gate; n++)
    {
      e[n] = S + (1 - S) * exp(-5.0 * (n - A) / D);
    }
  }
  else
  {
    for (n = A; n < gate; n++)
    {
      e[n] = S;
    }
  }

  if (R != 0)
  {
    for (n = gate; n < duration; n++)
    {
      e[n] = e[gate - 1] * exp(-5.0 * (n - gate + 1) / R);
    }
  }
}

void gen_fm(double freq)
{
    MONO_PCM pcm;
    int n, A, D, R, gate, duration;
    double *ac, fc, *am, fm, ratio, gain, S;
    double num;

    pcm.fs = 44100; /* 標本化周波数 */
    pcm.bits = 16; /* 量子化精度 */
//    pcm.length = OUTPUT_FRAMES; /* 音データの長さ */
//    pcm.s = (double*)calloc(pcm.length, sizeof(double)); /* 音データ */

//    ac = (double*)calloc(pcm.length, sizeof(double));
//    am = (double*)calloc(pcm.length, sizeof(double));

    /* キャリア振幅 */
    gate = pcm.fs * 4;
    duration = pcm.fs * 4;
    A = 0;
    D = pcm.fs * 4;
    S = 0.0;
    R = pcm.fs * 4;
//    ADSR(ac, A, D, S, R, gate, duration);

//    fc = freq; /* キャリア周波数 */
    fc = 440.0; /* キャリア周波数 */

    /* モジュレータ振幅 */
    gate = pcm.fs * 4;
    duration = pcm.fs * 4;
    A = 0;
    D = pcm.fs * 2;
    S = 0.0;
    R = pcm.fs * 2;
//    ADSR(am, A, D, S, R, gate, duration);

    ratio = 3.5;
    fm = fc * ratio; /* モジュレータ周波数 */

    /* FM音源 */
//    for (n = 0; n < pcm.length; n++)
    for (n = 0; n < OUTPUT_FRAMES; ++n)
    {
    	//num = ac[n] * sin(2.0 * M_PI * fc * n / pcm.fs + am[n] * sin(2.0 * M_PI * fm * n / pcm.fs));
    	num = sin(2.0 * M_PI * fc * n / pcm.fs * sin(2.0 * M_PI * fm * n / pcm.fs));
    	outputBuffer[n] = num * 10000;
    	keyon++;
		LOGI("keyon:%d", keyon);
#if 0
    	//ログを出力
    	__android_log_print(ANDROID_LOG_DEBUG,"soundtest","sawtooth:%f",num);
    	__android_log_print(ANDROID_LOG_DEBUG,"soundtest","sawtooth:%f",outputBuffer[n]);
#endif
	}

//    free(pcm.s);
//    free(ac);
//    free(am);

}

#endif /* FM_H_ */
