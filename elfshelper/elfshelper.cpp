#include "pch.h"
#include "elfshelper.h"

// RANDIT *******************

dll::RANDIT::RANDIT()
{
	std::random_device rd{};
	std::seed_seq sq{ rd(),rd(), rd(), rd(), rd(), rd(), rd(), rd() };

	twister = new std::mt19937(sq);
}
dll::RANDIT::~RANDIT()
{
	if (twister)delete twister;
}
int dll::RANDIT::operator()(int min, int max)
{
	if (min >= max)return 0;

	std::uniform_int_distribution distrib(min, max);

	return distrib(*twister);
}

/////////////////////////////

// PROTON *******************

dll::PROTON::PROTON(float _sx, float _sy)
{
	start.x = _sx;
	start.y = _sy;

	_width = 1.0f;
	_height = 1.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	radiusX = _width / 2;
	radiusY = _height / 2;

	center.x = start.x + radiusX;
	center.y = start.y + radiusY;
}
dll::PROTON::PROTON(float _sx, float _sy, float _swidth, float _sheight)
{
	start.x = _sx;
	start.y = _sy;

	_width = _swidth;
	_height = _sheight;

	end.x = start.x + _width;
	end.y = start.y + _height;

	radiusX = _width / 2;
	radiusY = _height / 2;

	center.x = start.x + radiusX;
	center.y = start.y + radiusY;
}
dll::PROTON::PROTON(float _sx, float _sy, float _swidth, float _sheight, PROTON*& _return_pointer)
{
	_return_pointer = new PROTON(_sx, _sy, _swidth, _sheight);
	in_heap = true;
}

float dll::PROTON::GetWidth() const
{
	return _width;
}
float dll::PROTON::GetHeight() const
{
	return _height;
}
bool dll::PROTON::InHeap() const
{
	return in_heap;
}

void dll::PROTON::SetEdges()
{	
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + radiusX;
	center.y = start.y + radiusY;
}
void dll::PROTON::SetWidth(float new_width)
{
	_width = new_width;
	
	end.x = start.x + _width;
	
	radiusX = _width / 2;
	
	center.x = start.x + radiusX;
}
void dll::PROTON::SetHeight(float new_height)
{
	_height = new_height;

	end.y = start.y + _height;

	radiusY = _height / 2;

	center.y = start.y + radiusY;
}
void dll::PROTON::NewDims(float new_width, float new_height)
{
	_width = new_width;
	_height = new_height;

	end.x = start.x + _width;
	end.y = start.y + _height;

	radiusX = _width / 2;
	radiusY = _height / 2;

	center.x = start.x + radiusX;
	center.y = start.y + radiusY;
}

bool dll::PROTON::Release()
{
	if (in_heap)
	{
		delete this;
		return true;
	}

	return false;
}

/////////////////////////////