#pragma once
#include "Shape.h"
#include <vector>
#include "Vector.h"



class Circle : public Shape {
public:
	Circle(Vector3 v1, Vector3 c);
	Vector3 origin;
	Vector3 point;

	virtual void Draw() override;
	void SetPoint(Vector3 p);
	

};
