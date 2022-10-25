#ifndef ROBOT_H
#define ROBOT_H
#include <set>
#include "hlt.hpp"
#define WAIT_TIME 5
#include <unordered_map>

class Robot
{
public:
	unsigned char id;
	hlt::GameMap &map;

	Robot(hlt::GameMap &map, unsigned char id);

	void save_strength();

	void assign_moves(int, int, int &);

	void get_details();

	std::set<hlt::Move> get_set();

private:
	std::vector<std::vector<int>> strength_map;
	std::vector<std::vector<int>> direction_map;

	bool acces_enemy(const hlt::Location &loc);

	bool enemy_border(const hlt::Location &loc);

	unsigned short value_site(const hlt::Location &loc);

	unsigned short value_site(const hlt::Site &site);

	int get_border(hlt::Location l, int);

	bool is_enemy_border(const hlt::Location &);

	unsigned char reverse_direction(unsigned char);

	unsigned short value_board_move(const hlt::Location &, unsigned char);

	unsigned short value_move(const hlt::Location &, unsigned char);

	unsigned short value_inner_move(const hlt::Location &, unsigned char);

	unsigned short max_move(const hlt::Location &);

	unsigned char optimize_board(const hlt::Location &);

	bool is_board(const hlt::Location &);
};

#endif
