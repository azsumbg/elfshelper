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

// TILE **********************

dll::TILE::TILE(tiles _what, float _sx, float _sy) :PROTON(_sx, _sy, 80.0f, 80.0f)
{
	type = _what;

	switch (type)
	{
	case tiles::grass1:
		tile_delay = 0;
		break;

	case tiles::grass2:
		tile_delay = 0.2f;
		break;

	case tiles::dirt:
		tile_delay = 0.3f;
		break;

	case tiles::path:
		tile_delay = -0.2f;
		break;

	case tiles::water:
		tile_delay = 10.0f;
		break;
	}
}
float dll::TILE::width() const
{
	return tile_width;
}
float dll::TILE::height() const
{
	return tile_height;
}
bool dll::TILE::Move(float gear)
{
	float my_speed = tile_speed += gear / 10;

	switch (dir)
	{
	case dirs::left:
		start.x -= my_speed;
		SetEdges();
		if (end.x <= -scr_width)return false;
		break;

	case dirs::right:
		start.x += my_speed;
		SetEdges();
		if (start.x <= 2 * scr_width)return false;
		break;

	case dirs::up:
		start.y -= my_speed;
		SetEdges();
		if (end.y <= -scr_height)return false;
		break;

	case dirs::down:
		start.y += my_speed;
		SetEdges();
		if (start.y <= 2 * scr_height)return false;
		break;
	}

	return true;
}
float dll::TILE::delay() const
{
	return tile_delay;
}
void dll::TILE::Release()
{
	delete this;
}

/////////////////////////////

//ASSETS ********************

dll::ASSETS::ASSETS(float _where_x, float _where_y, float _new_speed = 1.0f) :PROTON(_where_x, _where_y)
{
	speed = _new_speed;
}
bool dll::ASSETS::Move(float gear)
{
	float my_speed = speed += gear / 10;

	switch (dir)
	{
	case dirs::left:
		start.x -= my_speed;
		SetEdges();
		if (end.x <= -scr_width)return false;
		break;

	case dirs::right:
		start.x += my_speed;
		SetEdges();
		if (start.x <= 2 * scr_width)return false;
		break;

	case dirs::up:
		start.y -= my_speed;
		SetEdges();
		if (end.y <= -scr_height)return false;
		break;

	case dirs::down:
		start.y += my_speed;
		SetEdges();
		if (start.y <= 2 * scr_height)return false;
		break;
	}

	return true;
}

//////////////////////////////

// HOUSES ********************

dll::HOUSES::HOUSES(int _what_type, float _wherex, float _wherey, float _newspeed = 1.0f) :ASSETS(_wherex, _wherey, _newspeed)
{
	type = static_cast<houses>(_what_type);

	switch (type)
	{
	case houses::blue:
		NewDims(75.0f, 70.0f);
		break;

	case houses::red:
		NewDims(75.0f, 70.0f);
		break;

	case houses::purple:
		NewDims(75.0f, 70.0f);
		break;

	case houses::green:
		NewDims(75.0f, 70.0f);
		break;

	case houses::yellow:
		NewDims(75.0f, 70.0f);
		break;

	case houses::brown:
		NewDims(75.0f, 70.0f);
		break;
	}
}
void dll::HOUSES::Release()
{
	delete this;
}

//////////////////////////////

// OBSTACLES ****************

dll::OBSTACLES::OBSTACLES(int _what_type, float _wherex, float _wherey, float _newspeed) :ASSETS(_wherex, _wherey, _newspeed)
{
	type = static_cast<obstacles>(_what_type);

	switch (type)
	{
	case obstacles::rock1:
		NewDims(47.0f, 80.0f);
		break;

	case obstacles::rock2:
		NewDims(39.0f, 80.0f);
		break;

	case obstacles::tree1:
		NewDims(35.0f, 60.0f);
		break;

	case obstacles::tree2:
		NewDims(59.0f, 65.0f);
		break;

	case obstacles::tree3:
		NewDims(60.0f, 55.0f);
		break;
	}
}
void dll::OBSTACLES::Release()
{
	delete this;
}

/////////////////////////////

// EVILS *******************

int dll::EVILS::evil_id = 0;





//////////////////////////

// FUNCTION DEFINITIONS *******************************

bool dll::Intersect(FPOINT first, FPOINT second, float x1_radius, float x2_radius,
	float y1_radius, float y2_radius)
{
	return ((abs(first.x - second.x) <= x1_radius + x2_radius) && (abs(first.y - second.y) <= y1_radius + y2_radius));
}

dll::TILE* ELFS_API dll::TileFactory(tiles what, float sx, float sy)
{
	TILE* ret{ nullptr };
	ret = new TILE(what, sx, sy);
	return ret;
}

float dll::Distance(FPOINT first_center, FPOINT second_center)
{
	float a{ (float)(pow(abs(second_center.x - first_center.x), 2)) };
	float b{ (float)(pow(abs(second_center.y - first_center.y), 2)) };

	return (float)(sqrt(a + b));
}

bool dll::Sort(BAG<FPOINT>& container, FPOINT RefPoint)
{
	if (container.size() < 2)return false;

	bool sorted = false;
	
	while (!sorted)
	{
		sorted = true;

		for (size_t ind = 0; ind < container.size() - 1; ++ind)
		{
			if (Distance(container[ind], RefPoint) > Distance(container[ind + 1], RefPoint))
			{
				FPOINT temp = container[ind];
				container[ind] = container[ind + 1];
				container[ind + 1] = temp;

				sorted = false;
			}
		}
	}

	return true;
}