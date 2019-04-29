#pragma once
class AWVec
{
public:
	AWVec();
	~AWVec();
};

template <typename FLOAT>
class AWVec2
{
	FLOAT x, y;
public:
	AWVec2() : x(0.0), y(0.0) {}
	AWVec2(FLOAT _x, FLOAT _y) : x(_x), y(_y) {}
	~AWVec2() {}

	FLOAT X() {	return x; }
	FLOAT Y() {	return y; }
};

template <typename FLOAT>
class AWVec3
{
	FLOAT x, y, z;
public:
	AWVec3() : x(0.0), y(0.0), z(0.0) {}
	AWVec3(FLOAT _x, FLOAT _y, FLOAT _z) : x(_x), y(_y), z(_z) {}
	~AWVec3() {}

	FLOAT X() {	return x; }
	FLOAT Y() {	return y; }
	FLOAT Z() {	return z; }
};

