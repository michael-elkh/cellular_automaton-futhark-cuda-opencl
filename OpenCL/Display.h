#ifndef Display_H_
#define Display_H_

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

struct color{
	float r;
	float g;
	float b;
};

template <class T>
class Display {
public:

	Display( T* data, unsigned int sizex, unsigned int sizey, T max );
	Display();

	~Display();

	void show();

	char waitForKey();

	static void groundColorMix( color &col, float x, float min, float max );

private:

	T *data_;
	unsigned int sizeX_;
	unsigned int sizeY_;
	unsigned int max_;
	cv::Mat img_;
	bool hollow_;

};
#include "Display.cpp"

#endif /* Display_H_ */
