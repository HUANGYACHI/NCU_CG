#pragma once
#include <GL/glut.h>
#include <GL/gl.h>
#include "Vector.h"
#include <vector>
#include <string>
#define PI 3.1415926

void drawSquare(int x, int y,Vector3 &color);
void drawLine(int x0, int y0, int x1, int y1, Vector3 &color);
void drawCircle(int x0, int y0, int x1, int y1, Vector3 &color);
void drawRect(int x0, int y0, int w, int h, Vector3 &color);
float dist(int x0, int y0, int x1, int y1);
const std::vector<std::string> split(const std::string& str, const std::string& pattern);
Vector3 compute_intersection(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);
bool inside_line(float x, float y, Vector3 p1, Vector3 p2);
float map(float i, float x1, float y1, float x2, float y2);
std::vector<Vector3> clipping_algorithm(const Vector3* v, const std::vector<Vector3> polygon_points);
bool pnpoly(float x, float y, std::vector<Vector3> b);
Vector3 HSVtoRGB(float H, float S, float V);
