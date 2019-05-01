#pragma once

template <typename Type>
class AWVec2
{
	Type x, y;
public:
	AWVec2() : x(0.0), y(0.0) {}
	AWVec2(Type _x, Type _y) : x(_x), y(_y) {}
	~AWVec2() {}

	const Type X() const { return x; }
	const Type Y() const { return y; }

	/*MUSTDO: Check you AWVec2 is template class, for example float * double? */
	const AWVec2<Type>& operator=(const AWVec2<Type>& the)
	{
		x = the.X();
		y = the.Y();
		return *this;
	}

	AWVec2<Type> operator*(const AWVec2<Type>& the)
	{
		AWVec2<Type> result((x*the.X()), (x*the.Y()));
		return result;
	}
	/*MUSTDO: Check you AWVec2 is template class, case double? */
	AWVec2<Type> operator*(const float the)
	{
		AWVec2<Type> result((x*the), (y*the));
		return result;
	}

	AWVec2<Type> operator/(const AWVec2<Type>& the)
	{
		AWVec2<Type> result((x / the.X()), y / the.Y());
		return result;
	}

	AWVec2<Type> operator/(const float the)
	{
		AWVec2<Type> result((x / the), (y / the));
	}
};

template <typename Type>
class AWVec3
{
	Type x, y, z;
public:
	AWVec3() : x(0.0), y(0.0), z(0.0) {}
	AWVec3(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) {}
	~AWVec3() {}

	const Type X() const {	return x; }
	const Type Y() const {	return y; }
	const Type Z() const {	return z; }

	const AWVec3<Type>& operator=(const AWVec3<Type>& the)
	{
		x = the.X();
		y = the.Y();
		z = the.Z();
		return *this;
	}

	AWVec3<Type> operator*(const AWVec3<Type>& the)
	{
		AWVec3<Type> result(
			(x*the.X()),
			(y*the.Y()),
			(z*the.Z())
		);
		return result;
	}

	AWVec3<Type> operator*(const float the)
	{
		AWVec3<Type> result(
			(x*the),
			(y*the),
			(z*the)
		);
		return result;
	}

	AWVec3<Type> operator/(const float the)
	{
		AWVec3<Type> result(
			(x/the),
			(y/the),
			(z/the)
		);
		return result;
	}

	AWVec3<Type> operator/(const AWVec3<Type>& the)
	{
		AWVec3<Type> result(
			(x / the.X()),
			(y / the.Y()),
			(z / the.Z())
		);
		return result;
	}

};

class AWVec
{
public:
	AWVec();
	~AWVec();
};