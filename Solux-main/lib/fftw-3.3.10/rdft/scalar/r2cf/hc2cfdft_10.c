/*
 * Copyright (c) 2003, 2007-14 Matteo Frigo
 * Copyright (c) 2003, 2007-14 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Tue Sep 14 10:46:36 EDT 2021 */

#include "rdft/codelet-rdft.h"

#if defined(ARCH_PREFERS_FMA) || defined(ISA_EXTENSION_PREFERS_FMA)

/* Generated by: ../../../genfft/gen_hc2cdft.native -fma -compact -variables 4 -pipeline-latency 4 -n 10 -dit -name hc2cfdft_10 -include rdft/scalar/hc2cf.h */

/*
 * This function contains 122 FP additions, 92 FP multiplications,
 * (or, 68 additions, 38 multiplications, 54 fused multiply/add),
 * 81 stack variables, 5 constants, and 40 memory accesses
 */
#include "rdft/scalar/hc2cf.h"

static void hc2cfdft_10(R *Rp, R *Ip, R *Rm, R *Im, const R *W, stride rs, INT mb, INT me, INT ms)
{
     DK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DK(KP618033988, +0.618033988749894848204586834365638117720309180);
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * 18); m < me; m = m + 1, Rp = Rp + ms, Ip = Ip + ms, Rm = Rm - ms, Im = Im - ms, W = W + 18, MAKE_VOLATILE_STRIDE(40, rs)) {
	       E T3, T1u, Td, T1w, T1S, T2f, T14, T1p, T1j, T1q, T1N, T2e, TQ, T2i, T1n;
	       E T1H, Tz, T2h, T1m, T1C;
	       {
		    E T1, T2, T1h, Tc, TW, T1c, T1d, T1b, T1f, T1g, T1Q, T7, TV, T1J, TS;
		    E TU, Ts, Tx, T19, T18, T1O, T15, T17, Tt, T1A, Ti, Tn, TE, TD, T1F;
		    E TA, TC, Tj, T1y, TJ, TO, T12, T11, T1L, TY, T10, TK, T1D;
		    {
			 E Ta, Tb, T1e, T5, T6, TT;
			 T1 = Ip[0];
			 T2 = Im[0];
			 T1h = T1 + T2;
			 Ta = Rp[WS(rs, 2)];
			 Tb = Rm[WS(rs, 2)];
			 Tc = Ta - Tb;
			 TW = Ta + Tb;
			 T1c = Rm[0];
			 T1d = Rp[0];
			 T1e = T1c - T1d;
			 T1b = W[0];
			 T1f = T1b * T1e;
			 T1g = W[1];
			 T1Q = T1g * T1e;
			 T5 = Ip[WS(rs, 2)];
			 T6 = Im[WS(rs, 2)];
			 TT = T5 - T6;
			 T7 = T5 + T6;
			 TV = W[7];
			 T1J = TV * TT;
			 TS = W[6];
			 TU = TS * TT;
			 {
			      E Tq, Tr, T16, Tv, Tw, Tp;
			      Tq = Rm[WS(rs, 3)];
			      Tr = Rp[WS(rs, 3)];
			      Ts = Tq - Tr;
			      Tv = Ip[WS(rs, 3)];
			      Tw = Im[WS(rs, 3)];
			      Tx = Tv + Tw;
			      T16 = Tv - Tw;
			      T19 = Tr + Tq;
			      T18 = W[11];
			      T1O = T18 * T16;
			      T15 = W[10];
			      T17 = T15 * T16;
			      Tp = W[12];
			      Tt = Tp * Ts;
			      T1A = Tp * Tx;
			 }
			 {
			      E Tg, Th, TB, Tl, Tm, Tf;
			      Tg = Ip[WS(rs, 1)];
			      Th = Im[WS(rs, 1)];
			      Ti = Tg - Th;
			      Tl = Rp[WS(rs, 1)];
			      Tm = Rm[WS(rs, 1)];
			      Tn = Tl + Tm;
			      TB = Tm - Tl;
			      TE = Tg + Th;
			      TD = W[5];
			      T1F = TD * TB;
			      TA = W[4];
			      TC = TA * TB;
			      Tf = W[2];
			      Tj = Tf * Ti;
			      T1y = Tf * Tn;
			 }
			 {
			      E TH, TI, TZ, TM, TN, TG;
			      TH = Ip[WS(rs, 4)];
			      TI = Im[WS(rs, 4)];
			      TJ = TH - TI;
			      TM = Rp[WS(rs, 4)];
			      TN = Rm[WS(rs, 4)];
			      TO = TM + TN;
			      TZ = TN - TM;
			      T12 = TH + TI;
			      T11 = W[17];
			      T1L = T11 * TZ;
			      TY = W[16];
			      T10 = TY * TZ;
			      TG = W[14];
			      TK = TG * TJ;
			      T1D = TG * TO;
			 }
		    }
		    {
			 E T1P, T1R, T1K, T1M;
			 T3 = T1 - T2;
			 T1u = T1d + T1c;
			 {
			      E T4, T8, T9, T1v;
			      T4 = W[9];
			      T8 = T4 * T7;
			      T9 = W[8];
			      T1v = T9 * T7;
			      Td = FMA(T9, Tc, T8);
			      T1w = FNMS(T4, Tc, T1v);
			 }
			 T1P = FMA(T15, T19, T1O);
			 T1R = FMA(T1b, T1h, T1Q);
			 T1S = T1P - T1R;
			 T2f = T1P + T1R;
			 {
			      E TX, T13, T1a, T1i;
			      TX = FNMS(TV, TW, TU);
			      T13 = FNMS(T11, T12, T10);
			      T14 = TX + T13;
			      T1p = T13 - TX;
			      T1a = FNMS(T18, T19, T17);
			      T1i = FNMS(T1g, T1h, T1f);
			      T1j = T1a + T1i;
			      T1q = T1i - T1a;
			 }
			 T1K = FMA(TS, TW, T1J);
			 T1M = FMA(TY, T12, T1L);
			 T1N = T1K - T1M;
			 T2e = T1K + T1M;
			 {
			      E TF, T1G, TP, T1E, TL;
			      TF = FNMS(TD, TE, TC);
			      T1G = FMA(TA, TE, T1F);
			      TL = W[15];
			      TP = FNMS(TL, TO, TK);
			      T1E = FMA(TL, TJ, T1D);
			      TQ = TF + TP;
			      T2i = T1G + T1E;
			      T1n = TF - TP;
			      T1H = T1E - T1G;
			 }
			 {
			      E To, T1z, Ty, T1B, Tk, Tu;
			      Tk = W[3];
			      To = FNMS(Tk, Tn, Tj);
			      T1z = FMA(Tk, Ti, T1y);
			      Tu = W[13];
			      Ty = FNMS(Tu, Tx, Tt);
			      T1B = FMA(Tu, Ts, T1A);
			      Tz = To + Ty;
			      T2h = T1z + T1B;
			      T1m = Ty - To;
			      T1C = T1z - T1B;
			 }
		    }
	       }
	       {
		    E T2k, T2m, Te, T1l, T2b, T2c, T2l, T2d;
		    {
			 E T2g, T2j, TR, T1k;
			 T2g = T2e - T2f;
			 T2j = T2h - T2i;
			 T2k = FNMS(KP618033988, T2j, T2g);
			 T2m = FMA(KP618033988, T2g, T2j);
			 Te = T3 - Td;
			 TR = Tz + TQ;
			 T1k = T14 + T1j;
			 T1l = TR + T1k;
			 T2b = FNMS(KP250000000, T1l, Te);
			 T2c = TR - T1k;
		    }
		    Ip[0] = KP500000000 * (Te + T1l);
		    T2l = FMA(KP559016994, T2c, T2b);
		    Ip[WS(rs, 4)] = KP500000000 * (FMA(KP951056516, T2m, T2l));
		    Im[WS(rs, 3)] = -(KP500000000 * (FNMS(KP951056516, T2m, T2l)));
		    T2d = FNMS(KP559016994, T2c, T2b);
		    Ip[WS(rs, 2)] = KP500000000 * (FMA(KP951056516, T2k, T2d));
		    Im[WS(rs, 1)] = -(KP500000000 * (FNMS(KP951056516, T2k, T2d)));
	       }
	       {
		    E T2w, T2y, T2n, T2q, T2r, T2s, T2x, T2t;
		    {
			 E T2u, T2v, T2o, T2p;
			 T2u = T14 - T1j;
			 T2v = Tz - TQ;
			 T2w = FNMS(KP618033988, T2v, T2u);
			 T2y = FMA(KP618033988, T2u, T2v);
			 T2n = T1u + T1w;
			 T2o = T2h + T2i;
			 T2p = T2e + T2f;
			 T2q = T2o + T2p;
			 T2r = FNMS(KP250000000, T2q, T2n);
			 T2s = T2o - T2p;
		    }
		    Rp[0] = KP500000000 * (T2n + T2q);
		    T2x = FMA(KP559016994, T2s, T2r);
		    Rp[WS(rs, 4)] = KP500000000 * (FNMS(KP951056516, T2y, T2x));
		    Rm[WS(rs, 3)] = KP500000000 * (FMA(KP951056516, T2y, T2x));
		    T2t = FNMS(KP559016994, T2s, T2r);
		    Rp[WS(rs, 2)] = KP500000000 * (FNMS(KP951056516, T2w, T2t));
		    Rm[WS(rs, 1)] = KP500000000 * (FMA(KP951056516, T2w, T2t));
	       }
	       {
		    E T28, T2a, T1t, T1s, T23, T24, T29, T25;
		    {
			 E T26, T27, T1o, T1r;
			 T26 = T1H - T1C;
			 T27 = T1S - T1N;
			 T28 = FMA(KP618033988, T27, T26);
			 T2a = FNMS(KP618033988, T26, T27);
			 T1t = Td + T3;
			 T1o = T1m + T1n;
			 T1r = T1p + T1q;
			 T1s = T1o + T1r;
			 T23 = FMA(KP250000000, T1s, T1t);
			 T24 = T1r - T1o;
		    }
		    Im[WS(rs, 4)] = KP500000000 * (T1s - T1t);
		    T29 = FNMS(KP559016994, T24, T23);
		    Ip[WS(rs, 3)] = KP500000000 * (FMA(KP951056516, T2a, T29));
		    Im[WS(rs, 2)] = -(KP500000000 * (FNMS(KP951056516, T2a, T29)));
		    T25 = FMA(KP559016994, T24, T23);
		    Ip[WS(rs, 1)] = KP500000000 * (FMA(KP951056516, T28, T25));
		    Im[0] = -(KP500000000 * (FNMS(KP951056516, T28, T25)));
	       }
	       {
		    E T20, T22, T1x, T1U, T1V, T1W, T21, T1X;
		    {
			 E T1Y, T1Z, T1I, T1T;
			 T1Y = T1n - T1m;
			 T1Z = T1q - T1p;
			 T20 = FMA(KP618033988, T1Z, T1Y);
			 T22 = FNMS(KP618033988, T1Y, T1Z);
			 T1x = T1u - T1w;
			 T1I = T1C + T1H;
			 T1T = T1N + T1S;
			 T1U = T1I + T1T;
			 T1V = FNMS(KP250000000, T1U, T1x);
			 T1W = T1I - T1T;
		    }
		    Rm[WS(rs, 4)] = KP500000000 * (T1x + T1U);
		    T21 = FNMS(KP559016994, T1W, T1V);
		    Rp[WS(rs, 3)] = KP500000000 * (FMA(KP951056516, T22, T21));
		    Rm[WS(rs, 2)] = KP500000000 * (FNMS(KP951056516, T22, T21));
		    T1X = FMA(KP559016994, T1W, T1V);
		    Rp[WS(rs, 1)] = KP500000000 * (FMA(KP951056516, T20, T1X));
		    Rm[0] = KP500000000 * (FNMS(KP951056516, T20, T1X));
	       }
	  }
     }
}

static const tw_instr twinstr[] = {
     { TW_FULL, 1, 10 },
     { TW_NEXT, 1, 0 }
};

static const hc2c_desc desc = { 10, "hc2cfdft_10", twinstr, &GENUS, { 68, 38, 54, 0 } };

void X(codelet_hc2cfdft_10) (planner *p) {
     X(khc2c_register) (p, hc2cfdft_10, &desc, HC2C_VIA_DFT);
}
#else

/* Generated by: ../../../genfft/gen_hc2cdft.native -compact -variables 4 -pipeline-latency 4 -n 10 -dit -name hc2cfdft_10 -include rdft/scalar/hc2cf.h */

/*
 * This function contains 122 FP additions, 68 FP multiplications,
 * (or, 92 additions, 38 multiplications, 30 fused multiply/add),
 * 62 stack variables, 5 constants, and 40 memory accesses
 */
#include "rdft/scalar/hc2cf.h"

static void hc2cfdft_10(R *Rp, R *Ip, R *Rm, R *Im, const R *W, stride rs, INT mb, INT me, INT ms)
{
     DK(KP293892626, +0.293892626146236564584352977319536384298826219);
     DK(KP475528258, +0.475528258147576786058219666689691071702849317);
     DK(KP125000000, +0.125000000000000000000000000000000000000000000);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP279508497, +0.279508497187473712051146708591409529430077295);
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * 18); m < me; m = m + 1, Rp = Rp + ms, Ip = Ip + ms, Rm = Rm - ms, Im = Im - ms, W = W + 18, MAKE_VOLATILE_STRIDE(40, rs)) {
	       E Tw, TL, TM, T1W, T1X, T27, T1Z, T20, T26, TX, T1a, T1b, T1d, T1e, T1f;
	       E T1q, T1t, T1u, T1x, T1A, T1B, T1g, T1h, T1i, Td, T25, T1k, T1F;
	       {
		    E T3, T1D, T19, T1z, T7, Tb, TR, T1v, Tm, T1o, TK, T1s, Tv, T1p, T12;
		    E T1y, TF, T1r, TW, T1w;
		    {
			 E T1, T2, T18, T14, T15, T16, T13, T17;
			 T1 = Ip[0];
			 T2 = Im[0];
			 T18 = T1 + T2;
			 T14 = Rm[0];
			 T15 = Rp[0];
			 T16 = T14 - T15;
			 T3 = T1 - T2;
			 T1D = T15 + T14;
			 T13 = W[0];
			 T17 = W[1];
			 T19 = FNMS(T17, T18, T13 * T16);
			 T1z = FMA(T17, T16, T13 * T18);
		    }
		    {
			 E T5, T6, TO, T9, Ta, TQ, TN, TP;
			 T5 = Ip[WS(rs, 2)];
			 T6 = Im[WS(rs, 2)];
			 TO = T5 - T6;
			 T9 = Rp[WS(rs, 2)];
			 Ta = Rm[WS(rs, 2)];
			 TQ = T9 + Ta;
			 T7 = T5 + T6;
			 Tb = T9 - Ta;
			 TN = W[6];
			 TP = W[7];
			 TR = FNMS(TP, TQ, TN * TO);
			 T1v = FMA(TP, TO, TN * TQ);
		    }
		    {
			 E Th, TJ, Tl, TH;
			 {
			      E Tf, Tg, Tj, Tk;
			      Tf = Ip[WS(rs, 1)];
			      Tg = Im[WS(rs, 1)];
			      Th = Tf - Tg;
			      TJ = Tf + Tg;
			      Tj = Rp[WS(rs, 1)];
			      Tk = Rm[WS(rs, 1)];
			      Tl = Tj + Tk;
			      TH = Tj - Tk;
			 }
			 {
			      E Te, Ti, TG, TI;
			      Te = W[2];
			      Ti = W[3];
			      Tm = FNMS(Ti, Tl, Te * Th);
			      T1o = FMA(Te, Tl, Ti * Th);
			      TG = W[4];
			      TI = W[5];
			      TK = FMA(TG, TH, TI * TJ);
			      T1s = FNMS(TI, TH, TG * TJ);
			 }
		    }
		    {
			 E Tq, TZ, Tu, T11;
			 {
			      E To, Tp, Ts, Tt;
			      To = Ip[WS(rs, 3)];
			      Tp = Im[WS(rs, 3)];
			      Tq = To + Tp;
			      TZ = To - Tp;
			      Ts = Rp[WS(rs, 3)];
			      Tt = Rm[WS(rs, 3)];
			      Tu = Ts - Tt;
			      T11 = Ts + Tt;
			 }
			 {
			      E Tn, Tr, TY, T10;
			      Tn = W[13];
			      Tr = W[12];
			      Tv = FMA(Tn, Tq, Tr * Tu);
			      T1p = FNMS(Tn, Tu, Tr * Tq);
			      TY = W[10];
			      T10 = W[11];
			      T12 = FNMS(T10, T11, TY * TZ);
			      T1y = FMA(T10, TZ, TY * T11);
			 }
		    }
		    {
			 E TA, TV, TE, TT;
			 {
			      E Ty, Tz, TC, TD;
			      Ty = Ip[WS(rs, 4)];
			      Tz = Im[WS(rs, 4)];
			      TA = Ty - Tz;
			      TV = Ty + Tz;
			      TC = Rp[WS(rs, 4)];
			      TD = Rm[WS(rs, 4)];
			      TE = TC + TD;
			      TT = TC - TD;
			 }
			 {
			      E Tx, TB, TS, TU;
			      Tx = W[14];
			      TB = W[15];
			      TF = FNMS(TB, TE, Tx * TA);
			      T1r = FMA(Tx, TE, TB * TA);
			      TS = W[16];
			      TU = W[17];
			      TW = FMA(TS, TT, TU * TV);
			      T1w = FNMS(TU, TT, TS * TV);
			 }
		    }
		    Tw = Tm - Tv;
		    TL = TF - TK;
		    TM = Tw + TL;
		    T1W = T1v + T1w;
		    T1X = T1y + T1z;
		    T27 = T1W + T1X;
		    T1Z = T1o + T1p;
		    T20 = T1s + T1r;
		    T26 = T1Z + T20;
		    TX = TR - TW;
		    T1a = T12 + T19;
		    T1b = TX + T1a;
		    T1d = T19 - T12;
		    T1e = TR + TW;
		    T1f = T1d - T1e;
		    T1q = T1o - T1p;
		    T1t = T1r - T1s;
		    T1u = T1q + T1t;
		    T1x = T1v - T1w;
		    T1A = T1y - T1z;
		    T1B = T1x + T1A;
		    T1g = Tm + Tv;
		    T1h = TK + TF;
		    T1i = T1g + T1h;
		    {
			 E Tc, T1E, T4, T8;
			 T4 = W[9];
			 T8 = W[8];
			 Tc = FMA(T4, T7, T8 * Tb);
			 T1E = FNMS(T4, Tb, T8 * T7);
			 Td = T3 - Tc;
			 T25 = T1D + T1E;
			 T1k = Tc + T3;
			 T1F = T1D - T1E;
		    }
	       }
	       {
		    E T1U, T1c, T1T, T22, T24, T1Y, T21, T23, T1V;
		    T1U = KP279508497 * (TM - T1b);
		    T1c = TM + T1b;
		    T1T = FNMS(KP125000000, T1c, KP500000000 * Td);
		    T1Y = T1W - T1X;
		    T21 = T1Z - T20;
		    T22 = FNMS(KP293892626, T21, KP475528258 * T1Y);
		    T24 = FMA(KP475528258, T21, KP293892626 * T1Y);
		    Ip[0] = KP500000000 * (Td + T1c);
		    T23 = T1U + T1T;
		    Ip[WS(rs, 4)] = T23 + T24;
		    Im[WS(rs, 3)] = T24 - T23;
		    T1V = T1T - T1U;
		    Ip[WS(rs, 2)] = T1V + T22;
		    Im[WS(rs, 1)] = T22 - T1V;
	       }
	       {
		    E T2a, T28, T29, T2e, T2g, T2c, T2d, T2f, T2b;
		    T2a = KP279508497 * (T26 - T27);
		    T28 = T26 + T27;
		    T29 = FNMS(KP125000000, T28, KP500000000 * T25);
		    T2c = TX - T1a;
		    T2d = Tw - TL;
		    T2e = FNMS(KP293892626, T2d, KP475528258 * T2c);
		    T2g = FMA(KP475528258, T2d, KP293892626 * T2c);
		    Rp[0] = KP500000000 * (T25 + T28);
		    T2f = T2a + T29;
		    Rp[WS(rs, 4)] = T2f - T2g;
		    Rm[WS(rs, 3)] = T2g + T2f;
		    T2b = T29 - T2a;
		    Rp[WS(rs, 2)] = T2b - T2e;
		    Rm[WS(rs, 1)] = T2e + T2b;
	       }
	       {
		    E T1M, T1j, T1L, T1Q, T1S, T1O, T1P, T1R, T1N;
		    T1M = KP279508497 * (T1i + T1f);
		    T1j = T1f - T1i;
		    T1L = FMA(KP500000000, T1k, KP125000000 * T1j);
		    T1O = T1A - T1x;
		    T1P = T1q - T1t;
		    T1Q = FNMS(KP475528258, T1P, KP293892626 * T1O);
		    T1S = FMA(KP293892626, T1P, KP475528258 * T1O);
		    Im[WS(rs, 4)] = KP500000000 * (T1j - T1k);
		    T1R = T1L - T1M;
		    Ip[WS(rs, 3)] = T1R + T1S;
		    Im[WS(rs, 2)] = T1S - T1R;
		    T1N = T1L + T1M;
		    Ip[WS(rs, 1)] = T1N + T1Q;
		    Im[0] = T1Q - T1N;
	       }
	       {
		    E T1C, T1G, T1H, T1n, T1J, T1l, T1m, T1K, T1I;
		    T1C = KP279508497 * (T1u - T1B);
		    T1G = T1u + T1B;
		    T1H = FNMS(KP125000000, T1G, KP500000000 * T1F);
		    T1l = T1g - T1h;
		    T1m = T1e + T1d;
		    T1n = FMA(KP475528258, T1l, KP293892626 * T1m);
		    T1J = FNMS(KP293892626, T1l, KP475528258 * T1m);
		    Rm[WS(rs, 4)] = KP500000000 * (T1F + T1G);
		    T1K = T1H - T1C;
		    Rp[WS(rs, 3)] = T1J + T1K;
		    Rm[WS(rs, 2)] = T1K - T1J;
		    T1I = T1C + T1H;
		    Rp[WS(rs, 1)] = T1n + T1I;
		    Rm[0] = T1I - T1n;
	       }
	  }
     }
}

static const tw_instr twinstr[] = {
     { TW_FULL, 1, 10 },
     { TW_NEXT, 1, 0 }
};

static const hc2c_desc desc = { 10, "hc2cfdft_10", twinstr, &GENUS, { 92, 38, 30, 0 } };

void X(codelet_hc2cfdft_10) (planner *p) {
     X(khc2c_register) (p, hc2cfdft_10, &desc, HC2C_VIA_DFT);
}
#endif