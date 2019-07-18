////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C)  2015-2017, IVLab.
// 
// File name:     BasicImage.h
// 
// Author:        Vodka
// 
// Version:       V1.0
// 
// Date:          2015.09.22
// 
// Description:   CBASIC_IMAGE is designed as the advanced version of BASIC_IMAGE. Here, we use RAII to 
//                manage memory resource and model it as a template class. And we make it compatible with
//                the BASIC_IMAGE struct.
// 
// Others: 
//
// Example:      
//
//                CBASIC_IMAGE<uint8> image(200, 100); // Define a gray image with height = 200, width = 100
//                                                     // and its type is uint8.
//                
//                CBASIC_IMAGE<double> image(200, 100, 3) // Define a color image with height = 200, width =
//                                                        // 100, and its type is double.
// Function List:
//
//                1. operator BASIC_IMAGE* () const;
//                  We need to define implicit conversion from a wrapper(CBASIC_IMAGE) to a contained 
//                object(BASIC_IMAGE*), even though it's dangerous to use implicit conversion.
//                
//                2. 
// History: 
// 
// Modification:
// 2. ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <unordered_map>
#include <array>

using namespace std;

typedef unsigned __int64   uint64;
typedef unsigned long      uint32;
typedef unsigned short     uint16;             // 2-byte unsigned integer data type
typedef short              int16;
typedef unsigned char      uint8;              // 1-byte unsigned char type
typedef void *             lpvoid;

// --------------------------- N O T E ------------------------------
// ---- As (image->width * image->dim) will cost much time in calcultion
// ---- That is, we suggest NOT to use the macro IMijk directly when speed is desired.
// ---- Instead, we can use IMijk_x macro
#define IMij(image, type, i, j)     (((type *)(image->imv))[(i) * image->width * image->dim + (j) * image->dim])
#define IMijk(image, type, i, j, k) (((type *)(image->imv))[(i) * image->width * image->dim + (j) * image->dim + (k)])

// ---- NOTE:      'wid_x_dim' should be provided as 'image->width * image->dim'
#define IMijk_x(image, type, wid_x_dim, i, j, k) (((type *)(image->imv))[(i) * wid_x_dim + (j) * image->dim + (k)])


typedef enum _enumIMAGE_TYPE
{
	image_type_uint8 = 0,
	image_type_uint16,
	image_type_float,
	image_type_double
}IMAGE_TYPE;


// --------- the structure for the BASIC IMAGE data -------------
typedef struct _tagBASIC_IMAGE
{
	uint16 head_size;                      // size of the struct head, excepting the 'lpvoid'
	uint16 width;                          // image width
	uint16 height;                         // image height
	uint16 dim;                            // image dimension, typically 1 (gray), 3 (color), 16 (filter), 31 (reflectance)
	//	uint16 norm;
	uint16 image_type;                     // uint8, uint16, float, double

	lpvoid imv;                            // image pointer
}BASIC_IMAGE;



// ---------------------- NOTE: Please ensure that you are clear about how to use it correctly ------------ //
// ----------------------------------- vodka 2016.01.13 --------------------------------------------------- //
template<typename T>
class CBASIC_IMAGE
{
private:
	BASIC_IMAGE *__img;
	int __wid_x_channel;		// width x channel
	bool __dismiss;

	class Array2D;
	mutable Array2D __array2d;

	static unordered_map<string, IMAGE_TYPE> maptype;

	bool validtype(BASIC_IMAGE* image);
	void clone(BASIC_IMAGE* image);
	void destory();

	bool iswrapper() const
	{
		return (__dismiss == false);
	}

public:
	// Prevent heap allocation
	/*void* operator new  (size_t)   = delete;
	void* operator new[](size_t)   = delete;
	void  operator delete(void*)   = delete;
	void  operator delete[](void*) = delete;*/

	BASIC_IMAGE* create(int height, int width, int channel);

	CBASIC_IMAGE() :__img(nullptr), __wid_x_channel(0), __dismiss(true)
	{
		static_assert(is_same<T, uint8>::value
			|| is_same<T, uint16>::value
			|| is_same<T, float>::value
			|| is_same<T, double>::value
			|| is_same<T, array<int,256>>::value,
			"Illegal type: only can be 'uint8' or 'uint16' or 'float' or 'double'");
	}

	explicit CBASIC_IMAGE(int height, int width, int channel = 1) :CBASIC_IMAGE()
	{
		create(height, width, channel);
	}

	~CBASIC_IMAGE()
	{
		destory();
	}

	explicit CBASIC_IMAGE(BASIC_IMAGE* im, bool copyData = false) :CBASIC_IMAGE()
	{
		if (im)
		{
			assert(validtype(im));

			__dismiss = copyData;

			if (__dismiss)
			{
				create(im->height, im->width, im->dim);
				clone(im);
			}
			else
			{
				__img = im;
				__wid_x_channel = __img->width * __img->dim;
			}
		}
	}

	// Copy constructor
	CBASIC_IMAGE(const CBASIC_IMAGE<T> &rhs) :CBASIC_IMAGE()
	{
		create(rhs.height(), rhs.width(), rhs.channel());
		clone(rhs.getimage());
	}


	// Copy assignment operator
	/** ------------------------------- NOTE ----------------------------------------- /
		1. If the right hand side is a wrapper, then this object will also be a wrapper.
		2. If the right hand side is a real CBASIC_IMAGE<T> object and this object is a
		   a wrapper, we assert that the size of this object is not less than the rhs, 
		   if true, we simply copy data, if false, we re-create an image with size same
		   as the rhs then copy data.
	*/
	CBASIC_IMAGE<T> & operator = (const CBASIC_IMAGE<T> &rhs)
	{
		// self assignment
		if (this == &rhs)	// or use std::addressof
		{
			return *this;
		}

		if (!iswrapper())
		{
			destory();
		}
		
		if (rhs.iswrapper())
		{
			__img = rhs.__img;
			__wid_x_channel = rhs.width() * rhs.channel();
			__dismiss = false;
		}
		else
		{
			if (iswrapper())
			{
				assert(__wid_x_channel * height() >= rhs.height() * rhs.width() * rhs.channel());

				__img->height = rhs.height();
				__img->width = rhs.width();
				__img->dim = rhs.channel();
				__wid_x_channel = __img->width * __img->dim;
			}
			else
			{
				create(rhs.height(), rhs.width(), rhs.channel());
			}

			clone(rhs.getimage());
		}

		return *this;
	}

	// Move constructor
	CBASIC_IMAGE(CBASIC_IMAGE<T> && rhs) :CBASIC_IMAGE()
	{
		std::swap(__dismiss, rhs.__dismiss);
		std::swap(__img, rhs.__img);
		std::swap(__wid_x_channel, rhs.__wid_x_channel);
	}

	// Move assignment operator
	CBASIC_IMAGE<T> & operator = (CBASIC_IMAGE<T> && rhs)
	{
		if (!rhs.iswrapper() && iswrapper())
		{
			*this = rhs;	// use copy assignment
		} 
		else
		{  // including self assignment
			std::swap(__dismiss, rhs.__dismiss);
			std::swap(__img, rhs.__img);
			std::swap(__wid_x_channel, rhs.__wid_x_channel);
		}

		return *this;
	}

	// No bounds check for the sake of speed. Please use carefully!
	T& operator() (int row, int col, int channel = 0) const
	{
		return ((T *)(__img->imv))[row * __wid_x_channel + col * __img->dim + channel];
	}

	/** No bounds check for the sake of speed. Please use carefully!
	  For now, the operator (i, j, k) is faster than [i][j][k] when write data
	  and read data.
	*/
	__forceinline Array2D& operator[](int index)
	{
		__array2d.update2d(static_cast<T*>(__img->imv) + index * __wid_x_channel, __img->dim);

		return __array2d;
	}

	__forceinline const Array2D& operator[](int index) const
	{
		__array2d.update2d(static_cast<T*>(__img->imv) + index * __wid_x_channel, __img->dim);

		return __array2d;
	}

	BASIC_IMAGE* getimage() const
	{
		return __img;
	}

	T* data() const
	{
		return __img == nullptr ? nullptr : (T*)__img->imv;
	}

	// Implicit conversions
	operator BASIC_IMAGE * () const
	{
		return __img;
	}

	bool empty() const
	{
		return __img == nullptr;
	}

	uint16 width() const
	{
		return __img == nullptr ? 0 : __img->width;
	}

	uint16 height() const
	{
		return __img == nullptr ? 0 : __img->height;
	}

	uint16 channel() const
	{
		return __img == nullptr ? 0 : __img->dim;
	}

	IMAGE_TYPE type() const
	{
		return maptype[string(typeid(T).name())];
	}

	void update()
	{
		__wid_x_channel = this->width() * this->channel();
	}

	bool equalSize(const CBASIC_IMAGE<T>& im) const
	{
		return (__img == nullptr && im.__img == nullptr)
			|| (__img != nullptr && im.__img != nullptr
			                     && __img->height == im.height()
								 && __img->width == im.width()
								 && __img->dim == im.channel());
	}

	bool isIdentical(const CBASIC_IMAGE<T>& im) const
	{
		return this == &im;
	}

	int pixelNum() const
	{
		return __img == nullptr ? 0 : __img->height * __img->width * __img->dim;
	}

	int getBytes() const
	{
		return sizeof(T) * pixelNum();
	}

	// Take over the basic image from a dumb pointer and set the dumb pointer null
	const CBASIC_IMAGE<T>&  takeover(BASIC_IMAGE*& image);

	// Just wrap the basic image, but don't help it to manage its resource. 
	const CBASIC_IMAGE<T>&  wrapover(BASIC_IMAGE* image);
};

template<typename T>
unordered_map<string, IMAGE_TYPE> CBASIC_IMAGE<T>::maptype =
{
	{ string(typeid(uint8).name()), image_type_uint8 },
	{ string(typeid(uint16).name()), image_type_uint16 },
	{ string(typeid(float).name()), image_type_float },
	{ string(typeid(double).name()), image_type_double }
};

template<typename T>
BASIC_IMAGE* CBASIC_IMAGE<T>::create(int height, int width, int channel)
{
	if (width <= 0 || height <= 0 || channel <= 0)
	{
		destory();
		return nullptr;
	}


	const size_t size = width * height * channel;
	size_t prev_size = __img ? __img->width * __img->height * __img->dim : 0;
	// 对于所需空间大小与独占的原空间不等时才重新申请内存
	if (iswrapper() || prev_size != size)
	{
		destory();

		__dismiss = true;

		__img = new BASIC_IMAGE;
		__img->imv = new T[size]();
	}

	__img->head_size = sizeof(BASIC_IMAGE) - sizeof(lpvoid);  // 不计指针的结构体大小
	__img->width = width;
	__img->height = height;
	__img->dim = channel;
	__img->image_type = static_cast<uint16>(maptype[string(typeid(T).name())]);

	__wid_x_channel = width * channel;

	return __img;
}

template<typename T>
bool CBASIC_IMAGE<T>::validtype(BASIC_IMAGE* image)
{
	return image && static_cast<IMAGE_TYPE>(image->image_type) == maptype[string(typeid(T).name())];
}

template<typename T>
void CBASIC_IMAGE<T>::clone(BASIC_IMAGE* image)
{
	if (image && __img && image->imv && __img->imv)
	{
		memcpy(__img->imv, image->imv, sizeof(T) * image->width * image->height * image->dim);
	}
}

template<typename T>
void CBASIC_IMAGE<T>::destory()
{
	if (!iswrapper())
	{
		if (__img && __img->imv)
		{
			delete[](T *)__img->imv;
			__img->imv = NULL;
		}
		delete __img;
	}

	__img = nullptr;
	__wid_x_channel = 0;
}

template<typename T>
const CBASIC_IMAGE<T>& CBASIC_IMAGE<T>::takeover(BASIC_IMAGE*& image)
{
	destory();

	if (image)
	{
		assert(validtype(image));

		__dismiss = true;
		__img = image;
		image = nullptr;
		__wid_x_channel = __img->width * __img->dim;
	}

	return *this;
}

template<typename T>
const CBASIC_IMAGE<T>& CBASIC_IMAGE<T>::wrapover(BASIC_IMAGE* image)
{
	destory();

	if (image)
	{
		assert(validtype(image));

		__dismiss = false;
		__img = image;
		__wid_x_channel = __img->width * __img->dim;
	}

	return *this;
}


// Proxy class of CBASIC_IMAGE, return an Array2D object after call operator []
template<typename T>
class CBASIC_IMAGE<T>::Array2D
{
private:
	T* __data2d;
	int __channel;

	class Array1D;
	mutable Array1D __array1d;

public:
	explicit Array2D() : __data2d(nullptr), __channel(0){}

	__forceinline void update2d(T* data, int channel)
	{
		__data2d = data;
		__channel = channel;
	}

	// No bounds check for the sake of speed. Please use carefully!
	__forceinline Array1D& operator[](int index)
	{
		__array1d.update1d(__data2d + index *__channel);

		return __array1d;
	}

	__forceinline const Array1D& operator[](int index) const
	{
		__array1d.update1d(__data2d + index *__channel);

		return __array1d;
	}

	__forceinline T* operator & ()
	{
		return __data2d;
	}
};


// Proxy class of Array2D, return an Array1D object after call operator []
template<typename T>
class CBASIC_IMAGE<T>::Array2D::Array1D
{
private:
	T* __data1d;

public:
	explicit Array1D() : __data1d(nullptr){}

	__forceinline void update1d(T* data)
	{
		this->__data1d = data;
	}

	// No bounds check for the sake of speed. Please use carefully!
	__forceinline T& operator[] (int index)
	{
		return __data1d[index];
	}

	__forceinline const T& operator[] (int index) const
	{
		return __data1d[index];
	}

	// Implicit conversion
	__forceinline operator T& ()
	{
		return *__data1d;
	}

	__forceinline operator const T& () const
	{
		return *__data1d;
	}

	__forceinline T& operator = (T value)
	{
		*__data1d = value;

		return *__data1d;
	}

	__forceinline T* operator & ()
	{
		return __data1d;
	}
};