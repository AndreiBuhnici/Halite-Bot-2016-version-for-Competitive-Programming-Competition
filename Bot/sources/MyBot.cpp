#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"
#include "Robot.hpp"

/* Robotul propus */

int main()
{
    std::cout.sync_with_stdio(0);

    unsigned char my_id;
    hlt::GameMap my_map;
    getInit(my_id, my_map);
    int half_map = my_map.width * my_map.height / 2;
    short stage = 0;
    int delay = 5;

    Robot *bot = new Robot(my_map, my_id);

    sendInit("Odysseus-Robot");

    std::set<hlt::Move> moves;
    while (1)
    {
        moves.clear();

        getFrame(my_map);

        bot->get_details();

        int unobtained = 0;

        bot->assign_moves(stage, delay, unobtained);

        bot->save_strength();

        moves = bot->get_set();

        stage = unobtained / half_map;
        if (stage == 0)
            delay = 9;
        sendFrame(moves);
    }
    delete (bot);
    return 0;
}