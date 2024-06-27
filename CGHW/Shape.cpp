#include "Shape.h"


Shape::Shape() {
	color.Set(255, 255, 255);
}

void Shape::Draw() {


}


void Shape::SetColor(Vector3 &c) {
	color.Set(c.x,c.y,c.z);
}