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
  int fs; /* �W�{�����g�� */
  int bits; /* �ʎq�����x */
  int length; /* ���f�[�^�̒��� */
  double *s; /* ���f�[�^ */
} MONO_PCM;

#define OUTPUT_FRAMES 10
static short outputBuffer[OUTPUT_FRAMES];

static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

static int keyon = 0;

static int fmCount;

double ADSR(int A, int D, double S, int R, int gate, int duration)
{
  if (A != 0){
	  if (keyon < A) {
		  return 1.0 - exp(-5.0 * keyon / A);
	  }
  }

  if (D != 0) {
	  if (keyon >= A && (keyon < D && keyon < gate)) {
		  return S + (1 - S) * exp(-5.0 * (keyon - A) / D);
	  }
  }
  else {
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

void gen_fm(double fc)
{
    MONO_PCM pcm;
    int n;
    int A, D, R, gate, duration;
//    double *ac, fc, *am, ratio, gain, S;
    double S, ac, am, num, fm;

    pcm.fs = 44100; /* �W�{�����g�� */
    pcm.bits = 16; /* �ʎq�����x */
    pcm.length = OUTPUT_FRAMES; /* ���f�[�^�̒��� */
    //pcm.s = (double*)calloc(pcm.length, sizeof(double)); /* ���f�[�^ */

    //ac = (double*)calloc(pcm.length, sizeof(double));
    //am = (double*)calloc(pcm.length, sizeof(double));

#if 0
    /* �L�����A�U�� */
    gate = pcm.fs * 4;
    duration = pcm.fs * 4;
    A = 0;
    D = pcm.fs * 4;
    S = 0.0;
    R = pcm.fs * 4;
    ADSR(ac, A, D, S, R, gate, duration);

    fc = 440.0; /* �L�����A���g�� */

    /* ���W�����[�^�U�� */
    gate = pcm.fs * 4;
    duration = pcm.fs * 4;
    A = 0;
    D = pcm.fs * 2;
    S = 0.0;
    R = pcm.fs * 2;
    ADSR(am, A, D, S, R, gate, duration);

    ratio = 3.5;
    fm = fc * ratio; /* ���W�����[�^���g�� */
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
    D = pcm.fs * 2;
    S = 0.0;
    R = pcm.fs * 2;
    am = ADSR(A, D, S, R, gate, duration);

    fm = fc * 3.5;

    /* FM���� */
//    for (n = 0; n < pcm.length; n++)
    for (n = 0; n < OUTPUT_FRAMES; ++n)
    {
    	num = ac * sin(2.0 * M_PI * fc * keyon / pcm.fs + am * sin(2.0 * M_PI * fm * keyon / pcm.fs));
    	//num = sin(2.0 * M_PI * fc * keyon / pcm.fs * sin(2.0 * M_PI * fm * keyon / pcm.fs));
    	//num = sin(2.0 * M_PI * fc * keyon / pcm.fs);
    	outputBuffer[n] = num * 10000;

    	//���O���o��
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

#endif /* FM_H_ */
