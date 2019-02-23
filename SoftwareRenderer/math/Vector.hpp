
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <array>

template <typename T, int dim>
struct VectorData {
	union {
		std::array<T, dim> data;
	};
};
template <typename T>
struct VectorData<T, 4> {
	union {
		std::array<T, 4> data;
		T x, y, z, w;
	};
};
template <typename T>
struct VectorData<T, 3> {
	union {
		std::array<T, 3> data;
		T x, y, z;
	};
};
template <typename T>
struct VectorData<T, 2> {
	union {
		std::array<T, 2> data;
		T x, y;
	};
};
// Specialize for zero-length vectors
template <typename T>
struct VectorData<T, 0> { };

template <typename T, int dim>
class VectorBase : public VectorData<T, dim> {
public:
	// ------------------------
	// Constructors
	// ------------------------
	VectorBase() {}
	VectorBase(std::array<T, dim> _data)
	{
		memcpy(&this->data, &_data, sizeof(T)*dim);
	}

	// ------------------------
	// Length calculations
	// ------------------------
	float length() 
	{
		float result = 0;
		for (int i = 0; i < dim; ++i) 
		{
			result += this->data[i] * this->data[i];
		}
		return sqrt(result);
	}

	float lengthSquared()
	{
		float result = 0;
		for (int i = 0; i < dim; ++i)
		{
			result += this->data[i] * this->data[i];
		}
		return result;
	}

	// ------------------------
	// Normalization
	// ------------------------
	void normalize()
	{
		float len = length();
		for (int i = 0; i < dim; ++i)
		{
			this->data[i] /= len;
		}
	}
	VectorBase<T, dim> getNormalized()
	{
		float len = length();
		VectorBase<T, dim> result;
		for (int i = 0; i < dim; ++i)
		{
			result.data[i] = this->data[i] / len;
		}
		return result;
	}

	// ------------------------
	// Dot product
	// ------------------------
	float dot(const VectorBase<T, dim>& b)
	{
		float result = 0;
		for (int i = 0; i < dim; ++i)
		{
			result += this->data[i] * b.data[i];
		}
		return result;
	}

	// ------------------------
	// Component wise multiplication
	// ------------------------
	VectorBase<T, dim> componentWiseMul(const VectorBase<T, dim>& b)
	{
		VectorBase<T, dim> result;
		for (int i = 0; i < dim; ++i)
		{
			result.data[i] = this->data[i] * b.data[i];
		}
		return result;
	}
	void componentWiseMulEqual(const VectorBase<T, dim>& b)
	{
		for (int i = 0; i < dim; ++i) 
		{
			this->data[i] *= b.data[i];
		}
	}

	// ------------------------
	// Operators
	// ------------------------
	VectorBase<T, dim> operator+(const VectorBase<T, dim>& b)
	{
		VectorBase<T, dim> result;
		for (int i = 0; i < dim; ++i) {
			result.data[i] = this->data[i] + b.data[i];
		}
		return result;
	}
	VectorBase<T, dim> operator-(const VectorBase<T, dim>& b)
	{
		VectorBase<T, dim> result;
		for (int i = 0; i < dim; ++i) 
		{
			result.data[i] = this->data[i] - b.data[i];
		}
		return result;
	}
	// No multiplication operators since its ambiguous. Call the dot, cross or mul function directly!
	VectorBase<T, dim> operator/(const VectorBase<T, dim>& b)
	{
		VectorBase<T, dim> result;
		for (int i = 0; i < dim; ++i) {
			result.data[i] = this->data.data[i] / b.data[i];
		}
		return result;
	}
	void operator+=(const VectorBase<T, dim>& b)
	{
		for (int i = 0; i < dim; ++i) {
			this->data[i] += b.data[i];
		}
	}
	void operator-=(const VectorBase<T, dim>& b)
	{
		for (int i = 0; i < dim; ++i)
		{
			this->data[i] -= b.data[i];
		}
	}
	// Again, no multiplication-equal operators since its ambiguous. Call the dot, cross or mul function directly!
	void operator/=(const VectorBase<T, dim>& b)
	{
		for (int i = 0; i < dim; ++i) {
			this->data[i] /= b.data[i];
		}
	}

	// Operators for accessing data directly (easier for access in loops)
	const T operator[](unsigned i) const
	{
		return this->data[i];
	}
	T& operator[](unsigned i)
	{
		return this->data[i];
	}
	T& at(unsigned i)
	{
		// Bound checking
		static_assert(i < dim);
		return this->data[i];
	}

	// Check to see if vector is all zeros
	bool isNull()
	{
		for (int i = 0; i < dim; ++i) 
		{
			if (this->data[i])
				return false;
		}
		return true;
	}
	// Compare two vectors for equality
	bool isEqual(const VectorBase<T, dim>& b)
	{
		for (int i = 0; i < dim; ++i)
		{
			if (this->data[i] != b.data[i])
				return false;
		}
		return true;
	}
	// Print the vector onto the command line
	void print()
	{
		// This assumes atleast one component length vectors
		std::cout << this->data[0];
		for (int i = 1; i < dim; ++i)
		{
			std::cout << ", " << this->data[i];
		}
		std::cout << std::endl;
	}
};

// Specialize VectorBase for zero-length vectors, effectively disabling them to catch possible bugs earlier
template <typename T>
class VectorBase<T, 0> : public VectorData<T, 0> { };

// Specializations of common vectors for special functions and constructors
template <typename T, int dim>
class Vector {
public:
	Vector() {}
};
template <typename T>
class Vector<T, 4> : public VectorBase<T, 4> {
public:
	Vector() {}
	Vector(VectorBase<T, 4>&& base_copy) : VectorBase<T, 4>(std::move(base_copy)) {}
	Vector(T x, T y, T z, T w)
		: VectorBase<T, 4>({ x, y, z, w })
	{
	}
	Vector(std::array<T, 4>& data)
		: VectorBase<T, 4>(data)
	{
	}

	// Quaternion multiplication
	inline friend Vector<T, 4> cross(const Vector<T, 4>& a, const Vector<T, 4>& b)
	{
		Vector<T, 4> result;
		result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
		result.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
		result.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
		result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
		return result;
	}
	Vector<T, 4> cross(const Vector<T, 4>& b)
	{
		return cross(this, b);
	}
};
template <typename T>
class Vector<T, 3> : public VectorBase<T, 3> {
public:
	Vector() {}
	Vector(VectorBase<T, 3>&& base_copy) : VectorBase<T, 3>(std::move(base_copy)) {}
	Vector(T x, T y, T z)
		: VectorBase<T, 3>({ x, y, z })
	{
	}
	Vector(std::array<T, 3>& data)
		: VectorBase<T, 3>(data)
	{
	}

	// Cross product
	inline friend Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b)
	{
		Vector<T, 3> result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
		return result;
	}
	Vector<T, 3> cross(const Vector<T, 3>& b)
	{
		return cross(this, b);
	}
};
template <typename T>
class Vector<T, 2> : public VectorBase<T, 2> {
public:
	Vector() {}
	Vector(VectorBase<T, 2>&& base_copy) : VectorBase<T,2>(std::move(base_copy)) {}
	Vector(T x, T y)
		: VectorBase<T, 2>({ x, y })
	{
	}
	Vector(std::array<T, 2>& data)
		: VectorBase<T, 2>(data)
	{
	}

	// Cross product
	inline friend float cross(const Vector<T, 2>& a, const Vector<T, 2>& b)
	{
		return a.x * b.y - a.y * b.x;
	}
	Vector<T, 2> cross(const Vector<T, 2>& b)
	{
		return cross(this, b);
	}
};
// Zero length vectors are not supported; this helps catch bugs earlier
template <typename T>
class Vector<T, 0> : public VectorBase<T, 0> { };

// Typedef for some common vectors
typedef Vector<float, 4> Vec4f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 2> Vec2f;

typedef Vector<double, 4> Vec4d;
typedef Vector<double, 3> Vec3d;
typedef Vector<double, 2> Vec2d;

typedef Vec4f Vec4;
typedef Vec4f Quat;
typedef Vec4f Quaternion;
typedef Vec4f Quat4f;
typedef Vec4f Quat4;
typedef Vec3f Vec3;
typedef Vec2f Vec2;

#endif