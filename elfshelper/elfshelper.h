#pragma once

#ifdef ELFSHELPER_EXPORTS
#define ELFS_API __declspec(dllexport)
#else 
#define ELFS_API __declspec(dllimport)
#endif

#include <cstdlib>
#include <random>
#include <type_traits>

constexpr float scr_width{ 800.0f };
constexpr float scr_height{ 740.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 690.0f };

constexpr int MAX_FIELD_COLS{ 30 };
constexpr int MAX_FIELD_ROWS{ 28 };

constexpr int MAX_COLS{ 10 };
constexpr int MAX_ROWS{ 8 };

enum class dirs { stop = 0, left = 1, right = 2, up = 3, down = 4 };

enum class tiles { grass1 = 0, grass2 = 1, dirt = 2, water = 3, path = 4 };
enum class obstacles { tree1 = 0, tree2 = 1, tree3 = 2, rock1 = 3, rock2 = 4 };

enum class houses { blue = 0, red = 1, green = 2, yellow = 3, purple = 4, brown = 5 };

enum class evils { tree = 0, demon = 1, fly = 2, nasty = 3 };

enum class AI_actions { move = 0, attack = 1, patrol = 2, stop = 3};

struct ELFS_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};

struct ELFS_API STATUS
{
	float init_x{ 0 };
	float init_y{ 0 };

	float dest_x{ 0 };
	float dest_y{ 0 };

	AI_actions current_action{ AI_actions::stop };

	FPOINT obst_start{};
	FPOINT obst_end{};
	FPOINT obst_center{};
};

namespace dll
{
	struct ELFS_API PAIR
	{
		int col{ 0 };
		int row{ 0 };
	};

	class ELFS_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:

		RANDIT();
		~RANDIT();

		int operator()(int min, int max);
	};
	
	class ELFS_API PROTON
	{
	protected:
		float _width{ 0 };
		float _height{ 0 };
		bool in_heap = false;

	public:
		FPOINT start{};
		FPOINT end{};
		FPOINT center{};

		float radiusX{ 0 };
		float radiusY{ 0 };

		PROTON() {};
		PROTON(float _sx,float _sy);
		PROTON(float _sx, float _sy, float _swidth, float _sheight);
		PROTON(float _sx, float _sy, float _swidth, float _sheight, PROTON*& _return_pointer);

		virtual ~PROTON() {};

		float GetWidth() const;
		float GetHeight() const;
		bool InHeap() const;

		void SetEdges();
		void SetWidth(float new_width);
		void SetHeight(float new_height);
		void NewDims(float new_width, float new_height);

		bool Release();
	};

	class ELFS_API TILE :public PROTON
	{
	private:
		float tile_width = 80.0f;
		float tile_height = 80.0f;
		float tile_speed{ 1.0f };
		float tile_delay = { 0 };

	public:
		tiles type{ tiles::grass1 };
		dirs dir{ dirs::stop };

		int tile_number{ 0 };

		float width() const;
		float height() const;

		TILE(tiles _what, float _sx, float _sy);
		
		float delay() const;
		void Release();

	};

	class ELFS_API FIELD
	{
	private:
		RANDIT tile_choice{};

		int first_view_num = 0;
		
	public:
		TILE* FieldArray[MAX_FIELD_COLS][MAX_FIELD_ROWS];
		PAIR ViewPort[MAX_COLS * MAX_ROWS]{};


		dirs dir = dirs::stop;

		FIELD();
		~FIELD();

		int GetColFromNumber(int number);
		int GetRowFromNumber(int number);
		
		void MoveViewPort(float gear);

		void Release();

		void Recreate();
	};

	////////////////////////////////////////

	// CONTAINER **************************

	template<typename T> class BAG
	{
	private:
		T* base_ptr{ nullptr };
		size_t max_size{ 0 };
		size_t current_pos{ 0 };
		bool is_valid{ false };

	public:

		BAG() :max_size{ 1 }, base_ptr{ reinterpret_cast<T*>(calloc(max_size, sizeof(T))) }
		{
			if (base_ptr)is_valid = true;
		}
		BAG(size_t size):max_size{size}, base_ptr{ reinterpret_cast<T*>(calloc(max_size, sizeof(T))) }
		{
			if (base_ptr)is_valid = true;
		}
		BAG(size_t size,T first_element) :max_size{ size }, base_ptr{ reinterpret_cast<T*>(calloc(max_size, sizeof(T))) }
		{
			if (base_ptr)is_valid = true;
			(*base_ptr) = first_element;
		}

		~BAG()
		{
			if (base_ptr)free(base_ptr);
		}

		bool valid() const
		{
			return is_valid;
		}
		bool empty() const
		{
			return static_cast<bool>(current_pos);
		}
		size_t capacity()const
		{
			return max_size;
		}
		size_t size()const
		{
			return current_pos;
		}

		void push_front(T element)
		{
			if (base_ptr)
			{
				(*base_ptr) = element;
				++current_pos;
			}
		}
		void push_front(T* element)
		{
			if (base_ptr)
			{
				(*base_ptr) = (*element);
				++current_pos;
			}
		}

		void push_back(T element)
		{
			if (base_ptr)
			{
				if (current_pos < max_size)
				{
					base_ptr[current_pos] = element;
					++current_pos;
				}
				else
				{
					++max_size;
					base_ptr = reinterpret_cast<T*>(realloc(base_ptr, max_size * sizeof(T)));

					if (base_ptr)base_ptr[current_pos] = element;
					++current_pos;
				}
			}
		}
		void push_back(T* element)
		{
			if (base_ptr)
			{
				if (current_pos < max_size)
				{
					base_ptr[current_pos] = (*element);
					++current_pos;
				}
				else
				{
					++max_size;
					base_ptr = reinterpret_cast<T*>(realloc(base_ptr, max_size * sizeof(T)));

					if (base_ptr)base_ptr[current_pos] = (*element);
					++current_pos;
				}
			}
		}

		bool erase(size_t index)
		{
			if (index >= current_pos)return false;

			if (base_ptr)
			{
				T* temp_ptr{ reinterpret_cast<T*>(calloc(max_size - 1,sizeof(T))) };

				for (size_t count = 0; count < current_pos; ++count)
				{
					if (count < index)temp_ptr[count] = base_ptr[count];
					else temp_ptr[count] = base_ptr[count - 1];
				}

				--current_pos;
				--max_size;
				
				free(base_ptr);
				base_ptr = temp_ptr;
			}

			return true;
		}

		T front() const
		{
			T dummy{};

			if (base_ptr)return (*base_ptr);

			return dummy;
		}
		T back() const
		{
			T dummy{};
			if (base_ptr)return base_ptr[current_pos - 1];
			return dummy;
		}

		T operator[](size_t index)
		{
			if (base_ptr && index < current_pos)return base_ptr[index];

			return base_ptr[current_pos - 1];
		}

		bool operator()(size_t index, T element)
		{
			if (base_ptr && index < current_pos)
			{
				base_ptr[index] = element;
				return true;
			}
		
			return false;
		}
	};

	///////////////////////////////////////


	// ASSETS *****************************

	template<typename T> T* AssetsFactory(int what_type, float wherex, float wherey, float newspeed)
	{
		if constexpr (std::is_same<T, houses>::value || std::is_same<T, obstacles>::value)
			return new T(what_type, wherex, wherey, newspeed);

		return nullptr;
	};

	class ELFS_API ASSETS :public PROTON
	{
	protected:
		float speed = 1.0f;

	public:

		dirs dir = dirs::stop;

		ASSETS(float _where_x, float _where_y, float _new_speed);
		virtual ~ASSETS() {};

		bool Move(float gear);
		virtual void Release() = 0;
	};

	class ELFS_API HOUSES :public ASSETS
	{
	protected:
	
		HOUSES(int _what_type, float _wherex, float _wherey, float _newspeed);

	public:

		houses type{};

		void Release()override;

		friend HOUSES* AssetsFactory<HOUSES>(int what_type, float wherex, float wherey, float newspeed);
	};

	class ELFS_API OBSTACLES :public ASSETS
	{
	protected:

		OBSTACLES(int _what_type, float _wherex, float _wherey, float _newspeed);

	public:

		obstacles type{};

		void Release()override;

		friend OBSTACLES* AssetsFactory<OBSTACLES>(int what_type, float wherex, float wherey, float newspeed);
	};

	///////////////////////////////////////

	class ELFS_API EVILS :public PROTON
	{
	protected:
		static int evil_id;

		int my_id = 0;
		float speed = 0.2f;

		float slope{ 0 };
		float intercept{ 0 };

		bool vert_move{ false };
		bool hor_move{ false };

		int attack_delay{ 0 };
		int max_attack_delay{ 0 };

		int current_frame{ 0 };
		int max_frames{ 0 };
		int frame_delay{ 0 };
		int max_frame_delay{ 0 };

		int strenght = 0;
		float range{ 0 };

		EVILS(evils _what, float _start_x, float _start_y);

	public:
		evils type = evils::fly;
		int lifes = 0;
		STATUS status{};
		dirs dir = dirs::stop;

		float tile_delay = 0;

		int GetID()const;

		void SetID(int new_id);

		void SetMovePath(float to_where_x, float to_where_y);

		bool Move(float gear);

		int Attack();

		int GetFrame();

		void Release();

		friend ELFS_API EVILS* EvilFactory(evils what, float start_x, float start_y);
	
		friend ELFS_API void AINextMove(EVILS*& target_evil, BAG<OBSTACLES>& obstacle_arr, FPOINT hero_center, float game_speed);
	};

	class ELFS_API HERO:public PROTON
	{
	private:
		float speed{ 1.0f };

		int attack_delay{ 30 };
		int max_attack_delay{ 30 };

		int current_frame{ 0 };
		int max_frames{ 10 };
		int frame_delay{ 7 };
		int max_frame_delay{ 7 };

		int strenght = 0;

		bool in_heap{ false };

	public:
		dirs dir{ dirs::stop };
		int lifes = 150;
		bool in_battle = false;

		HERO(float _start_x, float start_y);

		bool Move(float gear);

		int Attack();

		int GetFrame();

		void Release();

		friend ELFS_API HERO* HeroFactory(float _start_x, float _start_y);
	};

	// TYPE DEFINITIONS **************************
	
	typedef EVILS* evil_ptr;

	typedef ASSETS* asset_ptr;

	typedef HERO* hero_ptr;

	// FUNCTION DECLARATIONS **************************

	ELFS_API bool Intersect(FPOINT first, FPOINT second, float x1_radius, float x2_radius,
		float y1_radius, float y2_radius1);

	ELFS_API EVILS* EvilFactory(evils what, float start_x, float start_y);

	ELFS_API void AINextMove(EVILS*& target_evil, BAG<OBSTACLES>& obstacle_arr, FPOINT hero_center, float game_speed);

	ELFS_API float Distance(FPOINT first_center, FPOINT second_center);

	ELFS_API bool Sort(BAG<FPOINT>& container, FPOINT RefPoint);

	ELFS_API HERO* HeroFactory(float _start_x, float _start_y);
}