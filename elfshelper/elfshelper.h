#pragma once

#ifdef ELFSHELPER_EXPORTS
#define ELFS_API __declspec(dllexport)
#else 
#define ELFS_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 800.0f };
constexpr float scr_height{ 740.0f };

constexpr int MAX_COLS{ 10 };
constexpr int MAX_ROWS{ 8 };

enum class dirs { stop = 0, left = 1, right = 2, up = 3, down = 4 };

enum class tiles { grass1 = 0, grass2 = 1, dirt = 2, water = 3, path = 4 };
enum class obstacles { tree1 = 0, tree2 = 1, tree3 = 2, rock1 = 3, rock2 = 4 };

enum class houses { blue = 0, red = 1, green = 2, yellow = 3, purple = 4, brown = 5 };

enum class evils { tree = 0, demon = 1, fly = 2, nasty = 3 };

struct ELFS_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};


namespace dll
{
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

		TILE(tiles _what, float _sx, float _sy);

	public:
		tiles type{ tiles::grass1 };
		dirs dir{ dirs::stop };

		float width() const;
		float height() const;

		float delay() const;

		bool Move(float gear);

		void Release();

		friend TILE* ELFS_API TileFactory(tiles what, float sx, float sy);
	};






	// FUNCTION DECLARATIONS **************************

	bool ELFS_API Intersect(FPOINT first, FPOINT second, float x1_radius, float x2_radius,
		float y1_radius, float y2_radius1);

	TILE* ELFS_API TileFactory(tiles what, float sx, float sy);
}