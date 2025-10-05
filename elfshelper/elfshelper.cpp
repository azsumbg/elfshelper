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

dll::TILE::TILE(tiles _what, float _sx, float _sy) :PROTON(_sx, _sy, 79.0f, 79.0f)
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
float dll::TILE::delay() const
{
	return tile_delay;
}
void dll::TILE::Release()
{
	delete this;
}

/////////////////////////////

// FIELD **********************

dll::FIELD::FIELD()
{
	int tile_number = 0;

	int tile_col = 0;
	int tile_row = 0;

	for (float y_pos = -720.0f; y_pos <= 2 * 720.0f; y_pos += 80.0f)
	{
		tile_col = 0;

		for (float x_pos = -scr_width; x_pos <= 2 * scr_width; x_pos += 80.0f)
		{
			FieldArray[tile_col][tile_row] = new TILE(static_cast<tiles>(tile_choice(0, 4)), x_pos, y_pos);

			FieldArray[tile_col][tile_row]->tile_number = tile_row * MAX_FIELD_COLS + tile_col;

			++tile_col;
		}

		++tile_row;
	}

	bool found{ false };
	
	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows]->start.x >= -80.0f && FieldArray[cols][rows]->start.y >= 0)
			{
				first_view_num = FieldArray[cols][rows]->tile_number;
				found = true;
				break;
			}
		}
		if (found)break;
	}
	
	int current_view_num = first_view_num;
	int last_col = GetColFromNumber(first_view_num) + 10;

	for (int i = 0; i < MAX_COLS * MAX_ROWS; ++i)
	{
		ViewPort[i].col = GetColFromNumber(current_view_num);
		ViewPort[i].row = GetRowFromNumber(current_view_num);
		if (ViewPort[i].col == last_col)current_view_num += MAX_FIELD_COLS - 11;
		else ++current_view_num;
	}
}
dll::FIELD::~FIELD()
{
	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows])delete FieldArray[cols][rows];
		}
	}
}
int dll::FIELD::GetColFromNumber(int number)
{
	if (number < 0 || number >= MAX_FIELD_COLS * MAX_FIELD_ROWS)return -2;

	int col_found = -1;

	bool found{ false };

	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows]->tile_number == number)
			{
				col_found = cols;
				found = true;
				break;
			}
		}
		if (found) break;
	}

	return col_found;
}
int dll::FIELD::GetRowFromNumber(int number)
{
	if (number < 0 || number >= MAX_FIELD_ROWS * MAX_FIELD_ROWS)return -2;

	int row_found = -1;

	bool found{ false };

	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows]->tile_number == number)
			{
				row_found = rows;
				found = true;
				break;
			}
		}
		if (found) break;
	}

	return row_found;
}
void dll::FIELD::MoveViewPort(float gear)
{
	switch (dir)
	{
	case dirs::up:
		down_end = true;
		if (FieldArray[0][MAX_FIELD_ROWS - 1]->end.y - gear > scr_height)
		{
			for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
			{
				for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
				{
					FieldArray[cols][rows]->start.y -= gear;
					FieldArray[cols][rows]->SetEdges();
					down_end = false;
				}
			}
		}
		
		break;

	case dirs::down:
		if (FieldArray[0][0]->start.y + gear < 0)
		{
			for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
			{
				for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
				{
					FieldArray[cols][rows]->start.y += gear;
					FieldArray[cols][rows]->SetEdges();
				}
			}
		}
		break;

	case dirs::left:
		right_end = true;
		if (FieldArray[MAX_FIELD_COLS - 1][0]->end.x - gear > scr_width)
		{
			for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
			{
				for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
				{
					FieldArray[cols][rows]->start.x -= gear;
					FieldArray[cols][rows]->SetEdges();
					right_end = false;
				}
			}
		}
		break;

	case dirs::right:
		if (FieldArray[0][0]->start.x + gear < 0)
		{
			for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
			{
				for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
				{
					FieldArray[cols][rows]->start.x += gear;
					FieldArray[cols][rows]->SetEdges();
				}
			}
		}
		break;
	}

	bool found = false;

	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows]->start.x >= -80.0f && FieldArray[cols][rows]->start.y >= 0)
			{
				first_view_num = FieldArray[cols][rows]->tile_number;
				found = true;
				break;
			}
		}
		if (found)break;
	}
	
	int current_view_num = first_view_num;
	int last_col = GetColFromNumber(first_view_num) + 10;

	for (int i = 0; i < MAX_COLS * MAX_ROWS; ++i)
	{
		ViewPort[i].col = GetColFromNumber(current_view_num);
		ViewPort[i].row = GetRowFromNumber(current_view_num);
		if (ViewPort[i].col == last_col)
		{
			if ((dir == dirs::up && down_end) || (dir == dirs::left && right_end))break;

			if (current_view_num + MAX_FIELD_COLS - 11 < MAX_FIELD_ROWS * MAX_FIELD_COLS)current_view_num += MAX_FIELD_COLS - 11;
			else break;
		}
		else
		{
			if (current_view_num + 1 < MAX_FIELD_ROWS * MAX_FIELD_COLS)++current_view_num;
			else break;
		}
	}
}
void dll::FIELD::Recreate()
{
	int tile_number = 0;

	int tile_col = 0;
	int tile_row = 0;

	for (float y_pos = -720.0f; y_pos <= 2 * 720.0f; y_pos += 80.0f)
	{
		for (float x_pos = -scr_width; x_pos <= 2 * scr_width; x_pos += 80.0f)
		{
			FieldArray[tile_col][tile_row] = new TILE(static_cast<tiles>(tile_choice(0, 4)), x_pos, y_pos);

			FieldArray[tile_col][tile_row]->tile_number = tile_row * MAX_FIELD_COLS + tile_col;

			++tile_col;
		}

		++tile_row;
	}

	bool found{ false };

	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows]->start.x >= -50.0f && FieldArray[cols][rows]->end.y >= 0)
			{
				first_view_num = FieldArray[cols][rows]->tile_number;
				found = true;
				break;
			}
		}
		if (found)break;
	}

	int current_view_num = first_view_num;
	int last_col = GetColFromNumber(first_view_num) + 10;

	for (int i = 0; i < MAX_COLS * MAX_ROWS; ++i)
	{
		ViewPort[i].col = GetColFromNumber(current_view_num);
		ViewPort[i].row = GetRowFromNumber(current_view_num);
		if (ViewPort[i].col == last_col)current_view_num += MAX_FIELD_COLS - 11;
		else ++current_view_num;
	}
}
void dll::FIELD::Release()
{
	for (int cols = 0; cols < MAX_FIELD_COLS; ++cols)
	{
		for (int rows = 0; rows < MAX_FIELD_ROWS; ++rows)
		{
			if (FieldArray[cols][rows])delete FieldArray[cols][rows];
		}
	}
}

///////////////////////////////

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

dll::EVILS::EVILS(evils _what, float _start_x, float _start_y) : PROTON(_start_x, _start_y), my_id{ evil_id }
{
	type = _what;
	++evil_id;

	switch (type)
	{
	case evils::tree:
		NewDims(100.0f, 84.0f);
		speed = 0.2f;
		lifes = 150;
		range = 150.0f;
		strenght = 40;
		attack_delay = 50;
		max_attack_delay = attack_delay;
		max_frames = 60;
		frame_delay = 1;
		if (start.x >= scr_width / 2)dir = dirs::left;
		else dir = dirs::right;
		break;

	case evils::demon:
		NewDims(51.0f, 70.0f);
		speed = 0.5f;
		lifes = 100;
		range = 220.0f;
		strenght = 30;
		attack_delay = 30;
		max_attack_delay = attack_delay;
		max_frames = 4;
		frame_delay = 12;
		if (start.x >= scr_width / 2)dir = dirs::left;
		else dir = dirs::right;
		break;

	case evils::fly:
		NewDims(60.0f, 60.0f);
		speed = 0.5f;
		lifes = 80;
		range = 250.0f;
		strenght = 20;
		attack_delay = 20;
		max_attack_delay = attack_delay;
		max_frames = 16;
		frame_delay = 4;
		if (start.x >= scr_width / 2)dir = dirs::left;
		else dir = dirs::right;
		break;

	case evils::nasty:
		NewDims(70.0f, 55.0f);
		speed = 0.4f;
		lifes = 100;
		range = 200.0f;
		strenght = 30;
		attack_delay = 30;
		max_attack_delay = attack_delay;
		max_frames = 23;
		frame_delay = 3;
		if (start.x >= scr_width / 2)dir = dirs::left;
		else dir = dirs::right;
		break;
	}

	max_frame_delay = frame_delay;
}
	
int dll::EVILS::GetID()const
{
	return my_id;
}

void dll::EVILS::SetID(int new_id)
{
	my_id = new_id;
}

void dll::EVILS::SetMovePath(float to_where_x, float to_where_y)
{
	status.init_x = start.x;
	status.init_y = start.y;

	status.dest_x = to_where_x;
	status.dest_y = to_where_y;

	hor_move = false;
	vert_move = false;

	if ((status.dest_x == status.init_x) || (status.dest_x > status.init_x && status.dest_x <= end.x))
	{
		vert_move = true;
		return;
	}
	if ((status.dest_y == status.init_y) || (status.dest_y > status.init_y && status.dest_y <= end.y))
	{
		hor_move = true;
		return;
	}

	slope = (status.dest_y - status.init_y) / (status.dest_x - status.init_x);
	intercept = start.y - slope * start.x;
}

bool dll::EVILS::Move(float gear)
{
	float now_speed = speed + gear / 10.0f - tile_delay;

	if (vert_move)
	{
		if (status.dest_y < status.init_y)
		{
			dir = dirs::up;
			start.y -= now_speed;
			SetEdges();
			if (start.y >= status.dest_y)
			{
				
				return false;
			}
			return true;
		}
		else if (status.dest_y > status.init_y)
		{
			dir = dirs::down;
			start.y += now_speed;
			SetEdges();
			if (start.y <= status.dest_y)
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (hor_move)
	{
		if (status.dest_x < status.init_x)
		{
			dir = dirs::left;
			start.x -= now_speed;
			SetEdges();
			if (start.x >= status.dest_x)
			{
				return false;
			}
			return true;
		}
		else if (status.dest_x > status.init_x)
		{
			dir = dirs::right;
			start.x += now_speed;
			SetEdges();
			if (start.x <= status.dest_x)
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	if (status.dest_x < status.init_x)
	{
		dir = dirs::left;
		start.x -= now_speed;
		start.y = start.x * slope + intercept;
		SetEdges();

		if (start.x <= status.dest_x || start.x <= 0 || end.x >= scr_width || start.y <= sky || end.y >= ground)
		{
			return false;
		}
	}
	else if (status.dest_x > status.init_x)
	{
		dir = dirs::right;
		start.x += now_speed;
		start.y = start.x * slope + intercept;
		SetEdges();

		if (start.x >= status.dest_x || start.x <= 0 || end.x >= scr_width || start.y <= sky || end.y >= ground)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

int dll::EVILS::Attack() 
{
	--attack_delay;
	if (attack_delay <= 0)
	{
		attack_delay = max_attack_delay;
		return strenght;
	}

	return 0;
}

int dll::EVILS::GetFrame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = max_frame_delay;

		++current_frame;
		if(current_frame >= max_frames)current_frame = 0;
	}

	return current_frame;
}

void dll::EVILS::Release()
{
	delete this;
}

//////////////////////////

// HERO **********************

dll::HERO::HERO(float _start_x, float _start_y) :PROTON(_start_x, _start_y)
{
	NewDims(70.0f, 62.0f);
}

bool dll::HERO::Move(float gear)
{
	if (in_battle)
	{
		in_battle = false;

		current_frame = 0;
		max_frames = 10;
		frame_delay = 7;
		max_frame_delay = 7;
	}

	float now_speed = speed + gear / 10.0f;

	switch (dir)
	{
	case dirs::up:
		if (start.y - now_speed <= sky)return false;
		start.y -= now_speed;
		SetEdges();
		break;

	case dirs::down:
		if (end.y + now_speed >= ground)return false;
		start.y += now_speed;
		SetEdges();
		break;

	case dirs::left:
		if (start.x - now_speed <= 0)return false;
		start.x -= now_speed;
		SetEdges();
		break;

	case dirs::right:
		if (end.x + now_speed >= scr_width)return false;
		start.x += now_speed;
		SetEdges();
		break;
	}

	return true;
}

int dll::HERO::Attack()
{
	if (!in_battle)
	{
		current_frame = 0;
		max_frames = 25;
		frame_delay = 3;
		max_frame_delay = 3;

		in_battle = true;
	}

	--attack_delay;

	if (attack_delay <= 0)
	{
		attack_delay = max_attack_delay;
		return strenght;
	}

	return 0;
}

int dll::HERO::GetFrame()
{
	--frame_delay;
	if (frame_delay < 0)
	{
		frame_delay = max_frame_delay;
		++current_frame;
		if (current_frame >= max_frames)current_frame = 0;
	}

	return current_frame;
}

void dll::HERO::Release()
{
	if (in_heap)delete this;
}

//////////////////////////////

// FUNCTION DEFINITIONS *******************************

bool dll::Intersect(FPOINT first, FPOINT second, float x1_radius, float x2_radius,
	float y1_radius, float y2_radius)
{
	return ((abs(first.x - second.x) <= x1_radius + x2_radius) && (abs(first.y - second.y) <= y1_radius + y2_radius));
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

ELFS_API dll::EVILS* dll::EvilFactory(evils what, float start_x, float start_y)
{
	EVILS* ret{ nullptr };

	ret = new EVILS(what, start_x, start_y);

	return ret;
}

ELFS_API dll::HERO* dll::HeroFactory(float _start_x, float _start_y)
{
	HERO* ret{ nullptr };

	ret = new HERO(_start_x, _start_y);

	if (ret)ret->in_heap = true;

	return ret;
}

ELFS_API void dll::AINextMove(EVILS*& target_evil, BAG<OBSTACLES>& obstacle_arr, FPOINT hero_center, float game_speed)
{
	RANDIT Choice{};

	unsigned char obstacle = 0;

	unsigned char right_obst = 0b00000001;
	unsigned char left_obst = 0b00000010;
	unsigned char down_obst = 0b00000100;
	unsigned char up_obst = 0b00001000;

	unsigned char up_right_obst = 0b00001001;
	unsigned char up_left_obst = 0b00001010;

	unsigned char down_right_obst = 0b00000101;
	unsigned char down_left_obst = 0b00000110;

	unsigned char inside_obst = 0b01000000;

	if (!obstacle_arr.empty())
	{
		for (size_t count = 0; count < obstacle_arr.size(); ++count)
		{
			if (Intersect(obstacle_arr[count].center, target_evil->center, obstacle_arr[count].radiusX, target_evil->radiusX,
				obstacle_arr[count].radiusY, target_evil->radiusY))
			{
				if (target_evil->start.x >= obstacle_arr[count].end.x)obstacle |= left_obst;
				if (target_evil->end.x <= obstacle_arr[count].start.x)obstacle |= right_obst;

				if (target_evil->start.y >= obstacle_arr[count].end.y)obstacle |= up_obst;
				if (target_evil->end.y <= obstacle_arr[count].start.y)obstacle |= down_obst;
				
				break;
			}
		}
	}

	switch (target_evil->status.current_action)
	{
	case AI_actions::stop:
		if (obstacle > 0)
		{
			if (obstacle == inside_obst)
			{
				if (target_evil->center.x < scr_width / 2)
				{
					target_evil->start.x += 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
				else
				{
					target_evil->start.x -= 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
			}
			else if (obstacle & up_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::down;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::up;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
		}
		if (Distance(target_evil->center, hero_center) > target_evil->range)  // PATROL
		{
			switch (Choice(0, 3))
			{
			case 0:
				target_evil->SetMovePath(target_evil->start.x - 200.0f, target_evil->start.y);
				target_evil->dir = dirs::left;
				break;

			case 1:
				target_evil->SetMovePath(target_evil->start.x + 200.0f, target_evil->start.y);
				target_evil->dir = dirs::right;
				break;

			case 2:
				target_evil->SetMovePath(target_evil->start.x, target_evil->start.y + 200.0f);
				target_evil->dir = dirs::down;
				break;

			case 3:
				target_evil->SetMovePath(target_evil->start.x, target_evil->start.y - 200.0f);
				target_evil->dir = dirs::up;
				break;
			}

			target_evil->status.current_action = AI_actions::patrol;
		}
		else
		{
			if (Intersect(target_evil->center, hero_center, target_evil->radiusX, 35.0f, target_evil->radiusY, 25.0f))  // ATTACK
				target_evil->status.current_action = AI_actions::attack;
			else
			{
				target_evil->SetMovePath(hero_center.x, hero_center.y);
				target_evil->status.current_action = AI_actions::move;
			}
		}
		break;

	case AI_actions::patrol:
		if (obstacle > 0)
		{
			if (obstacle == inside_obst)
			{
				if (target_evil->center.x < scr_width / 2)
				{
					target_evil->start.x += 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
				else
				{
					target_evil->start.x -= 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
			}
			else if (obstacle & up_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::down;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::up;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
		}
		if (!target_evil->Move(game_speed))
		{
			switch (target_evil->dir)
			{
			case dirs::left:
				target_evil->SetMovePath(target_evil->start.x + 200.0f, target_evil->start.y);
				target_evil->dir = dirs::right;
				break;

			case dirs::right:
				target_evil->SetMovePath(target_evil->start.x - 200.0f, target_evil->start.y);
				target_evil->dir = dirs::left;
				break;

			case dirs::up:
				target_evil->SetMovePath(target_evil->start.x, target_evil->start.y + 200.0f);
				target_evil->dir = dirs::down;
				break;

			case dirs::down:
				target_evil->SetMovePath(target_evil->start.x, target_evil->start.y - 200.0f);
				target_evil->dir = dirs::up;
				break;
			}
		}
		break;

	case AI_actions::move:
		if (obstacle > 0)
		{
			if (obstacle == inside_obst)
			{
				if (target_evil->center.x < scr_width / 2)
				{
					target_evil->start.x += 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
				else
				{
					target_evil->start.x -= 100.0f + (float)(Choice(20, 80));
					target_evil->SetEdges();
					break;
				}
			}
			else if (obstacle & up_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & left_obst)
			{
				target_evil->SetMovePath(target_evil->end.x + 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::right;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & right_obst)
			{
				target_evil->SetMovePath(target_evil->end.x - 100.0f + (float)(Choice(0, 50)),
					target_evil->end.y);
				target_evil->dir = dirs::left;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & up_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y + 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::down;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
			else if (obstacle & down_obst)
			{
				target_evil->SetMovePath(target_evil->end.x,
					target_evil->end.y - 100.0f + (float)(Choice(0, 50)));
				target_evil->dir = dirs::up;
				target_evil->status.current_action = AI_actions::move;
				break;
			}
		}
		if (Distance(target_evil->center, hero_center) > target_evil->range)  // CONTINUE
		{
			if (!target_evil->Move(game_speed))
			{
				target_evil->status.current_action = AI_actions::stop;
				break;
			}
		}
		else
		{
			if (Intersect(target_evil->center, hero_center, target_evil->radiusX, 35.0f, target_evil->radiusY, 25.0f))  // ATTACK
				target_evil->status.current_action = AI_actions::attack;
		}
		break;

	case AI_actions::attack:
		if (Intersect(target_evil->center, hero_center, target_evil->radiusX, 35.0f, target_evil->radiusY, 25.0f))  // ATTACK
			target_evil->status.current_action = AI_actions::attack;
		else target_evil->status.current_action = AI_actions::stop;
	}
}