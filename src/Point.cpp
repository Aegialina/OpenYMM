/**
 * \file
 * \brief Source code for the Point class
 * \author Copyright (c) 2011 Jonathan Thomas
 */

#include "../include/Point.h"

using namespace std;
using namespace openshot;

Point::Point(float x, float y) :
	interpolation(BEZIER), handle_type(AUTO) {
	// set new coorinate
	co = Coordinate(x, y);

	// set handles
	Initialize_Handles();
}

Point::Point(Coordinate co) :
	co(co), interpolation(BEZIER), handle_type(AUTO) {
	// set handles
	Initialize_Handles();
}

Point::Point(Coordinate co, Interpolation_Type interpolation) :
	co(co), interpolation(interpolation), handle_type(AUTO) {
	// set handles
	Initialize_Handles();
}

Point::Point(Coordinate co, Interpolation_Type interpolation, Handle_Type handle_type) :
	co(co), interpolation(interpolation), handle_type(handle_type) {
	// set handles
	Initialize_Handles();
}

void Point::Initialize_Handles(float Offset) {
	// initialize left and right handles
	handle_left = Coordinate(co.X - Offset, co.Y);
	handle_right = Coordinate(co.X + Offset, co.Y);
}