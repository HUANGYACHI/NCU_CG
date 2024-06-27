#pragma once
#include "Shape.h"
#include <vector>
#include "Vector.h"

class Poly : public Shape {
public:
	Poly();
	Poly(Vector3 v,Vector3 c);	
	virtual void Draw() override;
	void Draw(Vector3 r1, Vector3 r2,  Vector3 v1, Vector3 v2);
	void Add(Vector3 p);
	void Fill(std::vector<Vector3> p);
	std::vector<Vector3> vertex;
};

class Point : public Shape {
public:
	Point(Vector3 v,Vector3 c);	
	virtual void Draw() override;	
	Vector3 vertex;


};
