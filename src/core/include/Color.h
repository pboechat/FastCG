#ifndef COLOR_H
#define COLOR_H

enum Illuminant {
	A, 
	C, 
	D50, 
	D55, 
	D65, 
	D75, 
	F2, 
	F7, 
	F11, 
	I100, 
	n_illuminants
};

enum ColorModel
{
	CIE_XYZ,
	CIE_RGB,
	CIExyY,
	sRGB,
	CIELab
};

class Color
{
public:
	Color();
	~Color();

protected:
	ColorModel mModel;
	float* mpData;

};


#endif
