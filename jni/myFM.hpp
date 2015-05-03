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
  double* s; /* 音データ */
} MONO_PCM;

#define OUTPUT_FRAMES 10
#define PI 3.141592

class myFM
{
private:
	double freqNum;
	double ampNum;

public:
	double outputBuffer[OUTPUT_FRAMES];
	double* nextBuffer;
	unsigned nextSize;
	int nextCount;
	int keyon;
	int fmCount;
	//static double phase;

	myFM();
	virtual ~myFM();
	void genFM(double fc);
	void setFreq(double count);
	void setAmp(double count);
	void setTone();
	void prepareKeyOn();
	int getKeyOn();
	double ADSR(int A, int D, double S, int R, int gate, int duration);
};
#endif /* FM_H_ */
