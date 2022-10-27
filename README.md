# Halite-Bot-2016-version-for-Competitive-Programming-Competition
- First Place C++ Halite Bot 2016 ver. for Competitive Programming Competition hosted by ACS-UPB 2022
- The bot is based on the following strategies:
- The cells within the occupied terrain will search for the closest border, and if it doesn't exist they will stay in place;
- The cells on the border follow a greedy solution to attack all unoccupied cells;
- They will immediately attack any empty cell and if there are multiple empty cells, the one with the most score will be selected;
- If a border cell is unable to attack it will search for a cell with which it can fuse without overflow (it uses a strength map);
- The border cells prioretize the most vulnerable enemy cells;
- It uses a strenth loss correction method that, in esssence, it redirects power to all eligible cells to minimize loss;
