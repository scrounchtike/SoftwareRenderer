
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "Vector.hpp"

#include <array>
#include <cassert>

#undef min
#undef max
#undef near
#undef far

template <typename T, int dim_x, int dim_y>
class MatrixBase {
public:
	std::array<std::array<T, dim_y>, dim_x> data;

	// Common matrix constructors
	// Zero constructor
	inline static MatrixBase<T, dim_x, dim_y> initNull()
	{
		MatrixBase<T, dim_x, dim_y> matrix;
		memset(&matrix.data, 0, sizeof(T)*dim_x*dim_y);
		return matrix;
	}
	// Identity constructor
	inline static MatrixBase<T, dim_x, dim_y> initIdentity()
	{
		MatrixBase<T, dim_x, dim_y> matrix = std::move(initNull());
		for (int i = 0; i < std::min(dim_x, dim_y); ++i)
		{
			matrix.data[i][i] = 1;
		}
		return matrix;
	}
	// Scale constructor
	template <int dim>
	inline static MatrixBase<T, dim_x, dim_y> initScale(const Vector<T, dim>& scales)
	{
		MatrixBase<T, dim_x, dim_y> matrix = std::move(initIdentity());
		for (int i = 0; i < std::min(std::min(dim_x, dim_y), dim); ++i)
		{
			matrix.data[i][i] = scales[i];
		}
		return matrix;
	}
	// Translation constructor
	template <int dim>
	inline static MatrixBase<T, dim_x, dim_y> initTranslation(const Vector<T, dim>& translations)
	{
		MatrixBase<T, dim_x, dim_y> matrix = std::move(initIdentity());
		for (int i = 0; i < std::min(dim_y, dim); ++i)
		{
			matrix.data[dim_x - 1][i] = translations[i];
		}
		return matrix;
	}
	// 3D rotation constructor
	inline static MatrixBase<T, dim_x, dim_y> initRotation(T rx, T ry, T rz)
	{
		static_assert(dim_x < 3 && dim_y < 3, "Matrix needs to be atleast 3x3 for 3D rotation construction");

		MatrixBase<T, 3, 3> matrix_rx = std::move(initNull());
		matrix_rx.setRow(1, { 0, std::cos(rx), -std::sin(rx) });
		matrix_rx.setRow(2, { 0, std::sin(rx),  std::cos(rx) });
		MatrixBase<T, 3, 3> matrix_ry = std::move(initNull());
		matrix_ry.setRow(0, { std::cos(ry), 0, -std::sin(ry) });
		matrix_ry.setRow(2, { std::sin(ry), 0,  std::sin(ry) });
		MatrixBase<T, 3, 3> matrix_rz = std::move(initNull());
		matrix_rz.setRow(0, { std::cos(rz), -std::sin(rz), 0 });
		matrix_rz.setRow(1, { std::sin(rz),  std::cos(rz), 0 });

		MatrixBase<T, dim_x, dim_y> matrix = mul(mul(rx, ry), rz);
		return matrix;
	}
	// 2D rotation constructor
	inline static MatrixBase<T, dim_x, dim_y> initRotation(T theta)
	{
		static_assert(dim_x < 2 && dim_y < 2, "Matrix needs to be atleast 2x2 for 2D rotation construction");
		MatrixBase<T, dim_x, dim_y> matrix = std::move(initIdentity());
		matrix.setRow(0, { std::cos(theta), -std::sin(theta) });
		matrix.setRow(1, { std::sin(theta),  std::cos(theta) });
		return matrix;
	}
	// Camera matrix
	inline static MatrixBase<T, dim_x, dim_y> initCamera(const Vector<T, dim_y>& forward, const Vector<T, dim_y>& up, const Vector<T, dim_y>& right)
	{
		MatrixBase<T, dim_x, dim_y> matrix = std::move(initNull());
		// For now, only support for 3D camera matrices and 3D vectors (this is a 3D game engine after all..)
		// Vector components above the 3rd component will not be used (eg. w components are not used)
		static_assert("3D camera matrices only support matrices >= 3x3 and vectors with dimensions >= 3!");
		matrix.setColumn(0, right);
		matrix.setColumn(1, up);
		matrix.setColumn(2, forward);
		return matrix;
	}

	// Access functions
	Vector<T, dim_y> getColumn(unsigned i)
	{
		return Vector<T, dim_y>(data[i]);
	}
	Vector<T, dim_x> getRow(unsigned i)
	{
		std::array<T, dim_x> row;
		for (int c = 0; c < dim_x; ++c) {
			row[c] = data[c][i];
		}
		return row;
	}

	// Helper functions to set column and row vectors
	template <int dim>
	void setColumn(unsigned i, const Vector<T, dim>& column)
	{
		memcpy(&data[i], column, sizeof(T)*std::min(dim, dim_y));
	}
	template <int dim>
	void setRow(unsigned i, const Vector<T, dim>& row)
	{
		for (int c = 0; c < std::min(dim, dim_x); ++c)
		{
			data[c][i] = row[c];
		}
	}

	// Matrix multiplication (Same size matrices)
	inline friend MatrixBase<T, dim_x, dim_y> mul(const MatrixBase<T, dim_x, dim_y>& a, const MatrixBase<T, dim_x, dim_y>& b)
	{
		MatrixBase<T, dim_x, dim_y> result;
		for (int i = 0; i < dim_y; ++i)
		{
			for (int j = 0; j < dim_x; ++j)
			{
				result.data[i][j] = dot(getRow(i), b.getColumn(j));
			}
		}
		return result;
	}
	MatrixBase<T, dim_x, dim_y> mul(const MatrixBase<T, dim_x, dim_y>& b)
	{
		return mul(this, b);
	}
	// Matrix - matrix mul operators
	MatrixBase<T, dim_x, dim_y> operator*(const MatrixBase<T, dim_x, dim_y>& b)
	{
		return mul(this, b);
	}
	void operator*=(const MatrixBase<T, dim_x, dim_y>& b)
	{
		this = std::move(mul(this, b));
	}

	// Matrix vector multiplication
	template <int dim>
	inline friend Vector<T, dim> mul(const MatrixBase<T, dim_x, dim_y>& m, const Vector<T, dim>& v)
	{
		// For now, we only support matrix-vector multiplication for vectors that have atleast a dimension = or > than the number of rows of the matrix.
		// This is because, it is not clear if a vector with less components should or should not include the translation defined by a matrix with more columns!!
		// If the vector is greater, the components that are not affected by the matrix are simply copied. This makes it so that bigger matrices encapsulating smaller transformations can still be used.
		static_assert(dim < dim_x, "Vector dimension is smaller than matrix number of rows; it is ambiguous if the missing vector components should be extended with 0s or 1s (translation or not)");

		Vector<T, dim> result(v);
		for (int i = 0; i < std::min(dim, dim_y); ++i)
		{
			// dot product between the matrix row and the vector
			float dot = 0;
			Vector<T, dim_x> row = getRow(i);
			for (int j = 0; j < dim_x; ++j)
			{
				dot += row[j] * v[j];
			}
			result[i] = dot;
		}
		return result;
	}
	template <int dim>
	Vector<T, dim> mul(const Vector<T, dim> v)
	{
		return mul(this, v);
	}
	// Matrix - vector mul operator
	template <int dim>
	Vector<T, dim> operator*(const Vector<T, dim>& v)
	{
		return mul(this, v);
	}

	// Print function
	void print()
	{
		for (int i = 0; i < dim_y; ++i)
		{
			// Print every row on a new line
			
			std::cout << " [ ";
			std::cout << data[0][i];
			for (int j = 1; j < dim_x; ++j)
			{
				std::cout << ", " <<  data[j][i];
			}
			std::cout << " ] " << std::endl;
		}
	}
};
// Specialize for the zero dimension matrix so that we can catch bugs early
template <typename T, int dim_x>
class MatrixBase<T, 0, dim_x> {
public:
	MatrixBase()
	{
		assert(false);
	}
};
template <typename T, int dim_y>
class MatrixBase<T, dim_y, 0> {
public:
	MatrixBase()
	{
		assert(false);
	}
};

// Matrix specializations
template <typename T, int dim_x, int dim_y>
class Matrix : public MatrixBase<T, dim_x, dim_y> {
public:
	Matrix(const MatrixBase<T, dim_x, dim_y>&& matrix_copy) : MatrixBase<T, dim_x, dim_y>(std::move(matrix_copy)) {}
};
template <typename T>
class Matrix<T, 4, 4> : public MatrixBase<T, 4, 4> {
public:
	Matrix(const MatrixBase<T, 4, 4>&& matrix_copy) : MatrixBase<T, 4, 4>(std::move(matrix_copy)) {}

	// Projection matrix
	inline friend Matrix<T, 4, 4> initPerspective(T near, T far, T fov, T ar)
	{
		T tanHalfFOV = std::tan(fov/T(2.0));
		T range = far - near;
		Matrix<T, 4, 4> matrix;
		matrix.setRow(0, { ar*T(1.0) / tanHalfFOV, 0,                    0,                     0 });
		matrix.setRow(1, { 0,                      T(1.0) / tanHalfFOV,  0,                     0 });
		matrix.setRow(2, { 0,                      0,                   -(far - near) / range, -T(2.0)*far*near / range });
		matrix.setRow(3, { 0,                      0,                   -1,                     0 });
		return matrix;
	}
};

// Typedefs some common matrices
typedef Matrix<float, 4, 4> Mat4f;
typedef Matrix<float, 3, 3> Mat3f;
typedef Matrix<float, 2, 2> Mat2f;
typedef Matrix<float, 4, 3> Mat4x3f;
typedef Matrix<float, 3, 2> Mat3x2f;

typedef Matrix<double, 4, 4> Mat4d;
typedef Matrix<double, 3, 3> Mat3d;
typedef Matrix<double, 2, 2> Mat2d;
typedef Matrix<double, 4, 3> Mat4x3d;
typedef Matrix<double, 3, 2> Mat3x2d;

typedef Mat4f Mat4;
typedef Mat3f Mat3;
typedef Mat2f Mat2;
typedef Mat4x3f Mat4x3;
typedef Mat3x2f Mat3x2;

#endif