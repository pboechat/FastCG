#include "include/LightSpectrum.h"

// source: http://www.physics.kee.hu/cie/data/sid65.txt
const float LightSpectrum::ILLUMINANT_D65_10NM_SPECTRUM[] = { 51.03230f, 52.31180f, 68.70150f, 87.12040f, 92.45890f, 
															  90.05700f, 95.77360f, 110.93600f, 117.41000f, 116.33600f, 
															  115.39200f, 112.36700f, 109.08200f, 108.57800f, 106.29600f, 
															  106.23900f, 106.04700f, 104.22500f, 102.02300f, 98.16710f, 
															  96.06110f, 92.23680f, 89.34590f, 89.80260f, 88.64890f, 
															  85.49360f, 83.49390f, 81.86300f, 80.12070f, 81.24620f };

const LightSpectrum LightSpectrum::D65(10, 380, 730, ILLUMINANT_D65_10NM_SPECTRUM);