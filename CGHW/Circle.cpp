#include "Circle.h"

Circle::Circle(Vector3 v1,Vector3 c) {
	origin = v1;	
	color = c;
	enable = false;
}
void Circle::SetPoint(Vector3 p) {
	point = p;
	enable = true;
}





void Circle::Draw() {
	if (!enable) return;
	drawCircle(origin.x, origin.y, point.x, point.y,color);
}