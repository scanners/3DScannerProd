#include "stdafx.h"

#include "image.h"

Image::Image(Mat image, int imageNum) {

}

void Image::getRedComponent(Image image, int rowNum) {

}

void Image::process(Intrinsic intrinsic, Extrinsic backExt, Extrinsic groundExt) {

}

Image::~Image()
{
	if (back)
		delete back;
	if (ground)
		delete ground;
	if (laser)
		delete laser;
	if (bestFit)
		delete bestFit;
}