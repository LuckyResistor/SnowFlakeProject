//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
// https://luckyresistor.me
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "ValueArrays.hpp"


namespace ValueArrays {
	
	
const Fixed16 cRandom[256] = {
	Fixed16(0.4789f), Fixed16(0.4140f), Fixed16(0.1385f), Fixed16(0.9463f), Fixed16(0.7669f), Fixed16(0.5991f), Fixed16(0.2131f), Fixed16(0.1053f),
	Fixed16(0.2410f), Fixed16(0.3715f), Fixed16(0.6175f), Fixed16(0.4975f), Fixed16(0.8960f), Fixed16(0.3717f), Fixed16(0.9841f), Fixed16(0.3880f),
	Fixed16(0.5372f), Fixed16(0.8683f), Fixed16(0.2870f), Fixed16(0.7761f), Fixed16(0.9836f), Fixed16(0.2402f), Fixed16(0.9623f), Fixed16(0.8338f),
	Fixed16(0.5235f), Fixed16(0.6535f), Fixed16(0.2032f), Fixed16(0.7768f), Fixed16(0.5523f), Fixed16(0.7452f), Fixed16(0.2935f), Fixed16(0.8990f),
	Fixed16(0.0693f), Fixed16(0.3160f), Fixed16(0.9171f), Fixed16(0.1600f), Fixed16(0.1168f), Fixed16(0.3175f), Fixed16(0.7915f), Fixed16(0.5415f),
	Fixed16(0.1316f), Fixed16(0.2372f), Fixed16(0.7432f), Fixed16(0.9075f), Fixed16(0.4236f), Fixed16(0.4566f), Fixed16(0.6766f), Fixed16(0.6067f),
	Fixed16(0.6781f), Fixed16(0.0385f), Fixed16(0.8096f), Fixed16(0.4025f), Fixed16(0.9472f), Fixed16(0.3568f), Fixed16(0.1761f), Fixed16(0.9285f),
	Fixed16(0.8968f), Fixed16(0.4625f), Fixed16(0.2175f), Fixed16(0.0905f), Fixed16(0.1306f), Fixed16(0.0154f), Fixed16(0.7449f), Fixed16(0.7014f),
	Fixed16(0.1825f), Fixed16(0.9625f), Fixed16(0.0591f), Fixed16(0.7095f), Fixed16(0.2992f), Fixed16(0.6713f), Fixed16(0.7554f), Fixed16(0.7877f),
	Fixed16(0.8656f), Fixed16(0.4098f), Fixed16(0.0993f), Fixed16(0.3669f), Fixed16(0.5922f), Fixed16(0.0088f), Fixed16(0.0609f), Fixed16(0.2097f),
	Fixed16(0.9675f), Fixed16(0.7402f), Fixed16(0.6873f), Fixed16(0.9992f), Fixed16(0.7593f), Fixed16(0.0992f), Fixed16(0.7645f), Fixed16(0.2847f),
	Fixed16(0.6053f), Fixed16(0.4664f), Fixed16(0.0211f), Fixed16(0.4305f), Fixed16(0.4217f), Fixed16(0.5987f), Fixed16(0.6850f), Fixed16(0.2464f),
	Fixed16(0.8597f), Fixed16(0.6940f), Fixed16(0.0672f), Fixed16(0.0683f), Fixed16(0.7606f), Fixed16(0.5468f), Fixed16(0.1305f), Fixed16(0.5997f),
	Fixed16(0.9348f), Fixed16(0.1297f), Fixed16(0.9996f), Fixed16(0.1816f), Fixed16(0.6355f), Fixed16(0.7789f), Fixed16(0.7497f), Fixed16(0.0243f),
	Fixed16(0.3290f), Fixed16(0.9847f), Fixed16(0.3825f), Fixed16(0.9453f), Fixed16(0.9214f), Fixed16(0.5106f), Fixed16(0.4547f), Fixed16(0.7387f),
	Fixed16(0.2366f), Fixed16(0.4623f), Fixed16(0.1789f), Fixed16(0.1444f), Fixed16(0.2958f), Fixed16(0.4446f), Fixed16(0.8057f), Fixed16(0.0910f),
	Fixed16(0.3050f), Fixed16(0.8209f), Fixed16(0.8500f), Fixed16(0.4890f), Fixed16(0.3061f), Fixed16(0.0449f), Fixed16(0.3037f), Fixed16(0.7219f),
	Fixed16(0.0313f), Fixed16(0.7440f), Fixed16(0.9234f), Fixed16(0.7367f), Fixed16(0.6557f), Fixed16(0.1882f), Fixed16(0.5245f), Fixed16(0.2519f),
	Fixed16(0.4083f), Fixed16(0.2624f), Fixed16(0.9877f), Fixed16(0.7980f), Fixed16(0.8138f), Fixed16(0.4233f), Fixed16(0.0082f), Fixed16(0.8289f),
	Fixed16(0.1648f), Fixed16(0.9134f), Fixed16(0.0962f), Fixed16(0.0556f), Fixed16(0.9768f), Fixed16(0.6771f), Fixed16(0.9353f), Fixed16(0.2949f),
	Fixed16(0.0161f), Fixed16(0.0968f), Fixed16(0.8429f), Fixed16(0.4554f), Fixed16(0.5996f), Fixed16(0.0085f), Fixed16(0.6635f), Fixed16(0.2955f),
	Fixed16(0.1955f), Fixed16(0.5063f), Fixed16(0.2891f), Fixed16(0.7575f), Fixed16(0.4962f), Fixed16(0.7319f), Fixed16(0.3465f), Fixed16(0.6249f),
	Fixed16(0.5147f), Fixed16(0.7113f), Fixed16(0.1403f), Fixed16(0.7640f), Fixed16(0.7490f), Fixed16(0.6556f), Fixed16(0.0114f), Fixed16(0.2787f),
	Fixed16(0.4840f), Fixed16(0.0333f), Fixed16(0.4683f), Fixed16(0.2948f), Fixed16(0.1748f), Fixed16(0.2900f), Fixed16(0.7116f), Fixed16(0.3524f),
	Fixed16(0.1401f), Fixed16(0.2683f), Fixed16(0.9627f), Fixed16(0.8062f), Fixed16(0.0007f), Fixed16(0.7009f), Fixed16(0.7420f), Fixed16(0.5989f),
	Fixed16(0.7267f), Fixed16(0.1330f), Fixed16(0.5362f), Fixed16(0.6900f), Fixed16(0.8120f), Fixed16(0.9390f), Fixed16(0.5339f), Fixed16(0.3690f),
	Fixed16(0.3145f), Fixed16(0.3794f), Fixed16(0.7185f), Fixed16(0.8404f), Fixed16(0.0725f), Fixed16(0.1232f), Fixed16(0.4338f), Fixed16(0.2254f),
	Fixed16(0.6164f), Fixed16(0.7026f), Fixed16(0.3660f), Fixed16(0.9603f), Fixed16(0.2311f), Fixed16(0.9760f), Fixed16(0.3340f), Fixed16(0.6655f),
	Fixed16(0.4079f), Fixed16(0.8046f), Fixed16(0.0948f), Fixed16(0.7109f), Fixed16(0.7903f), Fixed16(0.6908f), Fixed16(0.6040f), Fixed16(0.5718f),
	Fixed16(0.4723f), Fixed16(0.7538f), Fixed16(0.7440f), Fixed16(0.7485f), Fixed16(0.0455f), Fixed16(0.9557f), Fixed16(0.5830f), Fixed16(0.2902f),
	Fixed16(0.5473f), Fixed16(0.4253f), Fixed16(0.1641f), Fixed16(0.6276f), Fixed16(0.7395f), Fixed16(0.9842f), Fixed16(0.3136f), Fixed16(0.8614f),
	Fixed16(0.3217f), Fixed16(0.1190f), Fixed16(0.5632f), Fixed16(0.2006f), Fixed16(0.7408f), Fixed16(0.0939f), Fixed16(0.4247f), Fixed16(0.3837f)
};


}