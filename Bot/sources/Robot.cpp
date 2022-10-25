#include <algorithm>
#include "hlt.hpp"
#include "Robot.hpp"
#include <queue>
#include <unordered_map>

bool Robot::is_board(const hlt::Location &loc)
{
	for (int i = 0; i < 4; i++)
	{
		if (map.getSite(loc, CARDINALS[i]).owner != id)
			return true;
	}
	return false;
}

bool Robot::acces_enemy(const hlt::Location &loc)
{
	unsigned char dist = 1;
	std::queue<hlt::Location> border;
	border.push(loc);
	hlt::Location MARKER{map.width, map.width};
	border.push(MARKER);
	std::unordered_map<unsigned short, unsigned char> visit;
	while (!border.empty() && dist < 4)
	{
		hlt::Location location = border.front();

		border.pop();
		if (location.x == map.width)
		{
			dist++;
			if (!border.empty())
				border.push(location);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				hlt::Location get_location = map.getLocation(location, CARDINALS[i]);
				hlt::Site get_site = map.getSite(get_location);
				unsigned short key = get_location.x + get_location.y * map.width;
				if (get_site.owner == id && (visit.find(key) == visit.end()))
				{
					border.push(get_location);
					visit[key] = 1;
				}
				else if (get_site.owner == 0 && get_site.strength == 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

unsigned short Robot::value_site(const hlt::Site &site)
{
	int max = 257;
	unsigned char strength = (site.owner == 0 ? site.strength : 0);
	if (site.production == 0 && strength != 0)
		return 1;
	return max + site.production - strength / (std::max)(site.production, (unsigned char)(1));
}

bool Robot::is_enemy_border(const hlt::Location &loc)
{
	hlt::Site site = map.getSite(loc);
	if (site.owner != 0 || site.strength == 0)
		return false;
	bool my_site = false;
	for (int i = 0; i < 4; i++)
	{
		hlt::Site get_site = map.getSite(loc, CARDINALS[i]);
		if (get_site.owner == id)
		{
			my_site = true;
		}
	}
	return my_site;
}

unsigned short Robot::value_site(const hlt::Location &loc)
{
	if (enemy_border(loc) && !acces_enemy(loc))
		return 1;

	unsigned short total_val = 0;
	hlt::Site site = map.getSite(loc);
	if (site.owner != id)
		total_val += value_site(site);
	unsigned short total_prod = site.production;
	for (int i = 0; i < 4; i++)
	{
		hlt::Site site = map.getSite(loc, CARDINALS[i]);
		if (site.owner != id)
		{
			unsigned short sdVal = value_site(site);
			total_val += (sdVal > 250 ? sdVal - 250 : 0);
			if (site.owner != 0)
			{
				total_val += 10;
			}
		}
		total_prod += site.production;
	}
	return total_val;
}

bool Robot::enemy_border(const hlt::Location &loc)
{
	hlt::Site site = map.getSite(loc);
	if (site.owner != 0 || site.strength == 0)
		return false;
	bool enemy = false, my_site = false;
	for (int i = 0; i < 4; i++)
	{
		hlt::Site nb = map.getSite(loc, CARDINALS[i]);
		if (nb.owner == id)
		{
			my_site = true;
		}
		else if (nb.owner != 0)
		{
			enemy = true;
		}
	}
	return my_site && enemy;
}

unsigned short Robot::max_move(const hlt::Location &loc)
{
	unsigned short max_val = 0;
	for (int i = 0; i < 4; i++)
	{
		unsigned short val = value_board_move(loc, CARDINALS[i]);
		if (val > max_val)
		{
			max_val = val;
		}
	}
	return max_val;
}

int Robot::get_border(hlt::Location l, int stage)
{
	int min_distance = (std::min)(map.width, map.height) / 2;
	int direction = STILL;
	unsigned short maxVal = (255 - map.getSite(l).strength) /
								std::max(map.getSite(l).production, (unsigned char)(1)) /
								2 +
							(6 + 4 * stage) * min_distance;

	for (int i = 0; i < 4; i++)
	{
		int distance = 0;
		int val = 0;
		hlt::Location current_tile = l;
		hlt::Site next = map.getSite(l, CARDINALS[i]);
		while (next.owner == id && distance < min_distance)
		{
			distance++;
			current_tile = map.getLocation(current_tile, CARDINALS[i]);
			next = map.getSite(current_tile);
		}
		if (next.owner != id)
		{
			short valb = value_site(current_tile);
			val = valb + (6 + 4 * stage) * (min_distance - distance);
		}
		if (val > maxVal)
		{
			direction = CARDINALS[i];
			maxVal = val;
		}
	}
	return direction;
}

Robot::Robot(hlt::GameMap &map, unsigned char id) : map(map)
{
	direction_map = std::vector<std::vector<int>>(map.height, std::vector<int>(map.width, 5));
	this->id = id;
	strength_map = std::vector<std::vector<int>>(map.height, std::vector<int>(map.width, 0));
}

/* Se salveaza puterea */
void Robot::save_strength()
{
	int max = 255;
	for (unsigned short i = 0; i < map.height; i++)
		for (unsigned short j = 0; j < map.width; j++)
		{
			hlt::Location location{j, i};
			if (strength_map[i][j] > max + 1 || map.getSite(location).owner == id)
			{
				unsigned char direction = (direction_map[i][j] & 15);
				std::vector<std::vector<unsigned char>> dirs;
				for (auto d : DIRECTIONS)
				{

					hlt::Location new_location = map.getLocation(location, d);
					if (map.getSite(new_location).owner == id && (direction_map[new_location.y][new_location.x] & 15) == reverse_direction(d))
					{
						dirs.push_back(std::vector<unsigned char>{static_cast<unsigned char>(d), map.getSite(new_location).strength});
					}
				}
				/* Euristica */
				sort(dirs.begin(), dirs.end(), [](const std::vector<unsigned char> &v1, const std::vector<unsigned char> &v2)
					 { return v1[1] < v2[1]; });
				hlt::Location new_location = map.getLocation(location, direction);
				hlt::Site new_site = map.getSite(new_location);
				hlt::Location rev_location = map.getLocation(location, reverse_direction(direction));
				hlt::Site rev_site = map.getSite(rev_location);
				if ((new_site.owner != 0 || new_site.strength == 0) && direction != 0 && direction < 5)
				{
					if ((strength_map[i][j] + map.getSite(location).strength < 256) &&
						((dirs.size() > 0 && (strength_map[i][j] + 150 > map.getSite(location).strength)) ||
						 ((direction_map[new_location.y][new_location.x] >> 4) == reverse_direction(direction) && new_site.owner == id)))
					{
						direction_map[i][j] = ((direction_map[i][j] | 15) & (240 + 0));
						strength_map[i][j] += map.getSite(location).strength;
						strength_map[new_location.y][new_location.x] -= map.getSite(location).strength;
					}
					else if (((direction_map[rev_location.y][rev_location.x] & 15) == direction) &&
							 (rev_site.strength > map.getSite(location).strength) &&
							 (strength_map[rev_location.y][rev_location.x] + map.getSite(location).strength < 256) &&
							 (rev_site.strength - map.getSite(location).strength > 40))
					{
						direction_map[i][j] = ((direction_map[i][j] | 15) & (240 + reverse_direction(direction)));
						strength_map[rev_location.y][rev_location.x] += map.getSite(location).strength;
						strength_map[new_location.y][new_location.x] -= map.getSite(location).strength;
					}
				}
				for (auto dir : dirs)
				{
					hlt::Location new_location = map.getLocation(location, dir[0]);
					if (strength_map[i][j] > 256)
					{
						if (dir[0] != 0 && strength_map[new_location.y][new_location.x] + dir[1] <= 256)
						{
							direction_map[new_location.y][new_location.x] = ((direction_map[new_location.y][new_location.x] | 15) & (240 + 0));
							strength_map[new_location.y][new_location.x] += dir[1];
							strength_map[i][j] -= dir[1];
						}
						else if (dir[0] == 0)
						{
							unsigned short strength = 255;
							unsigned char d = 0;
							for (auto dd : CARDINALS)
							{
								hlt::Location l = map.getLocation(location, dd);
								if (map.getSite(l).owner == id && (strength_map[l.y][l.x] < strength || ((strength_map[l.y][l.x] == strength) && (direction_map[l.y][l.x] & 15) == reverse_direction(dd))))
								{
									d = dd;
									strength = strength_map[l.y][l.x];
								}
							}
							hlt::Location l = map.getLocation(location, d);
							if (strength_map[l.y][l.x] + dir[1] < 256 && dir[1] > 0)
							{
								direction_map[i][j] = ((direction_map[i][j] | 15) & (240 + d));
								strength_map[l.y][l.x] += dir[1];
								strength_map[i][j] -= dir[1];
							}
						}
					}
				}
				direction = (direction_map[i][j] & 15);
				new_location = map.getLocation(location, direction);
				new_site = map.getSite(new_location);
				bool exist_enemy = false;
				if (direction != 0 && direction < 5 && new_site.owner == id && new_site.strength > 0)
				{
					unsigned char d = direction_map[new_location.y][new_location.x] & 15;
					hlt::Site nnSite = map.getSite(new_location, d);
					if (nnSite.owner == 0 && nnSite.strength == 0)
					{
						hlt::Location loc_s = map.getLocation(new_location, d);
						for (auto dd : CARDINALS)
						{
							hlt::Site nnnSite = map.getSite(loc_s, dd);
							if (nnnSite.owner != 0 && nnnSite.owner != id && nnnSite.strength > 0)
								exist_enemy = true;
						}
					}
				}
				bool second_enemy = (exist_enemy && map.getSite(location).strength + map.getSite(new_location).strength > 256 && map.getSite(location).strength < map.getSite(new_location).strength + 40);

				if (second_enemy)
				{
					direction_map[i][j] = ((direction_map[i][j] | 15) & (240 + 0));
					strength_map[i][j] += map.getSite(location).strength;
					strength_map[new_location.y][new_location.x] -= map.getSite(location).strength;
				}
				std::queue<hlt::Location> over_cap;
				if (strength_map[location.y][location.x] > 256)
					over_cap.push(location);
				while (!over_cap.empty())
				{
					hlt::Location loc_s = over_cap.front();
					over_cap.pop();
					std::vector<std::vector<unsigned char>> dirs;

					if (strength_map[loc_s.y][loc_s.x] > 256)
					{
						for (int i=0;i<4;i++)
						{
							hlt::Location new_location = map.getLocation(loc_s, CARDINALS[i]);
							if (map.getSite(new_location).owner == id && (direction_map[new_location.y][new_location.x] & 15) == reverse_direction(CARDINALS[i]))
							{
								dirs.push_back(std::vector<unsigned char>{static_cast<unsigned char>(CARDINALS[i]), map.getSite(new_location).strength});
							}
						}
						/* Euristica */
						std::sort(dirs.begin(), dirs.end(), [](const std::vector<unsigned char> &v1, const std::vector<unsigned char> &v2)
								  { return v1[1] < v2[1]; });
					}

					for (auto dir : dirs)
					{
						unsigned char d = dir[0];
						hlt::Location n_loc = map.getLocation(loc_s, d);
						if ((direction_map[n_loc.y][n_loc.x] & 15) == reverse_direction(d) && strength_map[loc_s.y][loc_s.x] > 256)
						{
							direction_map[n_loc.y][n_loc.x] = ((direction_map[n_loc.y][n_loc.x] | 15) & (240 + 0));
							strength_map[loc_s.y][loc_s.x] -= map.getSite(n_loc).strength;
							strength_map[n_loc.y][n_loc.x] += map.getSite(n_loc).strength;
							if (strength_map[n_loc.y][n_loc.x] > 256)
								over_cap.push(n_loc);
						}
					}
				}
			}
		}
}

unsigned short Robot::value_board_move(const hlt::Location &loc, unsigned char direction)
{
	hlt::Location new_location = map.getLocation(loc, direction);
	hlt::Site get_site = map.getSite(new_location);
	hlt::Site get_loc = map.getSite(loc);
	if (get_site.owner != get_loc.owner)
	{
		id = get_loc.owner;
		unsigned short damage = value_site(new_location);
		if (damage == 1)
			return 1;
		if (get_site.strength >= get_loc.strength)
		{
			unsigned short dmg_penalty = (get_site.strength - get_loc.strength) / (std::max)(get_site.production, (unsigned char)(1));
			if (damage > dmg_penalty)
				damage -= dmg_penalty;
			else
				return 0;
		}
		unsigned short bot_overkill = 0;
		bool exist_enemy = false;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site site = map.getSite(new_location, CARDINALS[i]);
			if (site.owner != 0 && site.owner != get_loc.owner)
			{
				bot_overkill += std::min(get_loc.strength, site.strength);
				exist_enemy = true;
			}
		}
		if (exist_enemy && bot_overkill <= get_loc.strength && get_site.strength > 0)
		{
			damage = 0;
		}
		else
		{
			damage += bot_overkill;
		}
		return damage;
	}
	else
		return 0;
}

unsigned short Robot::value_inner_move(const hlt::Location &loc, unsigned char direction)
{
	if (map.getSite(loc).strength <= map.getSite(loc).production * 4)
		return 0;
	bool exist_enemy = false;
	for (int i = 0; i < 4; i++)
	{
		hlt::Site site = map.getSite(loc, CARDINALS[i]);
		if (site.owner == 0 && site.strength == 0)
			exist_enemy = true;
	}
	if (exist_enemy)
		return 0;
	unsigned char id = map.getSite(loc).owner;
	unsigned char distance = 0;
	unsigned short max_dist = (std::min)(map.width, map.height) / 2;
	hlt::Location new_location = map.getLocation(loc, direction);
	unsigned short value = max_move(new_location);
	while (map.getSite(new_location).owner == id && distance < max_dist)
	{
		new_location = map.getLocation(new_location, direction);
		++distance;
	}
	unsigned short value_board = value_site(new_location);
	unsigned short distance_penalty = distance * 5;
	if (value_board > distance_penalty)
	{
		value_board -= distance_penalty;
	}
	else
	{
		value_board = 0;
	}
	if (value > 5)
	{
		value -= 5;
	}
	else
	{
		value = 0;
	}
	if (exist_enemy)
		return value_board;
	else
		return std::max(value, value_board);
}

unsigned short Robot::value_move(const hlt::Location &loc, unsigned char direction)
{
	hlt::Location new_location = map.getLocation(loc, direction);
	if (map.getSite(loc).owner != map.getSite(new_location).owner)
	{
		return value_board_move(loc, direction);
	}
	else
	{
		return value_inner_move(loc, direction);
	}
}

unsigned char Robot::optimize_board(const hlt::Location &loc)
{
	hlt::Site s = map.getSite(loc);
	unsigned short value_max = ((s.strength + s.production > 200) ? 0 : (1 + s.production * 10));
	unsigned char direction = 0;
	for (int i = 0; i < 4; i++)
	{
		unsigned short value = value_move(loc, CARDINALS[i]);
		if (value > value_max)
		{
			direction = CARDINALS[i];
			value_max = value;
		}
	}
	return direction;
}

void Robot::get_details()
{
	for (unsigned short i = 0; i < map.height; i++)
		for (unsigned short j = 0; j < map.width; j++)
		{
			direction_map[i][j] = ((direction_map[i][j] << 4) | 5);
			strength_map[i][j] = 0;
		}
}

void Robot::assign_moves(int stage, int delay, int &unobtained)
{
	for (unsigned short i = 0; i < map.height; i++)
		for (unsigned short j = 0; j < map.width; j++)
			if (map.getSite({j, i}).owner == id)
			{
				bool is_border = false;
				bool moved_piece = false;
				unsigned char dir = 0;
				unsigned char direction = 0;
				for (int k = 0; k < 4; k++)
				{
					if (map.getSite({j, i}, CARDINALS[k]).owner != id)
					{
						is_border = true;
					}
				}
				if (is_border)
				{
					direction = optimize_board({j, i});
					if (map.getSite({j, i}, direction).strength >= map.getSite({j, i}).strength && map.getSite({j, i}, direction).owner != id)
					{
						dir = 0;
					}
					else
					{
						dir = direction;
					}
					moved_piece = true;
				}

				if (!moved_piece && (map.getSite({j, i}).strength <= map.getSite({j, i}).production * delay))
				{
					dir = 0;
					moved_piece = true;
				}

				if (!moved_piece && !is_border)
				{
					unsigned char direction = get_border({j, i}, stage);
					dir = direction;
					moved_piece = true;
				}
				direction_map[i][j] = ((direction_map[i][j] | 15) & (240 + dir));
				hlt::Location new_location = map.getLocation({j, i}, dir);
				strength_map[new_location.y][new_location.x] += map.getSite({j, i}).strength;
			}
			else if (map.getSite({j, i}).owner == 0)
			{
				++unobtained;
			}
}

std::set<hlt::Move> Robot::get_set()
{
	std::set<hlt::Move> moves;
	for (unsigned short i = 0; i < map.height; ++i)
	{
		for (unsigned short j = 0; j < map.width; ++j)
		{
			hlt::Location location{j, i};
			if (map.getSite(location).owner == id)
			{
				unsigned char direction = (direction_map[i][j] & 15);
				moves.insert({location, direction});
			}
		}
	}
	return moves;
}

unsigned char Robot::reverse_direction(unsigned char direction)
{
	int value = 4;
	if (direction == 0)
		return 0;
	else if (direction > 5)
	{
		return direction + 1;
	}
	else
	{
		return (direction + 1) % value + 1;
	}
}
