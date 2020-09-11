



#define CHRWIDTH  5*2
#define CHRHEIGHT 8*2

#define SCRWIDTH  20
#define SCRHEIGHT 15

struct s_coords {
  int16_t x;
  int16_t y;
};

struct s_actor {
  uint8_t type;             // 0 - pacman, 1- ghost
  uint8_t direction;        // 0 - up, 1- right, 2 - down, 3 - left
  uint8_t lastdirection;    // direction where we came from
  uint8_t animCounter;      // for counting animation frames
  uint8_t stepsCounter;     // counting steps (pixels) when moving from one tile to another
  uint16_t color;
  s_coords coords;          // current coordinates
  s_coords lastcoords;      // last coordinates for deleting previous frame from display
  uint8_t lastFreeDirectionBits;    // bitmask for surrounding walls, to check if we are in an intersection
};

const s_coords DIRMATRIX[4] = { { 0, -1}, { 1, 0}, { 0, 1}, { -1, 0}, }; // directions for calculations - up, right, down, left
const s_coords DIRMATRIX_OPP[4] = { { 0, 1}, { -1, 0}, { 0, -1}, { 1, 0}, }; // opposite directions for calculations - up, right, down, left

// pacman moving frame sprites up,right,down, left

const unsigned char PACFRAMES[4][4] = {
  {0, 7, 8, 7}, 
  {0, 1, 2, 1},
  {0, 5, 6, 5},
  {0, 3, 4, 3}
};


// our cute actors :)
s_actor ghosts[4], pacman;


// string needs to be SCRWIDTH + one zero byte long (null terminated strings)
char MAP[SCRHEIGHT + 2][SCRWIDTH + 3] = {
  "                      ",
  " xxxxxxxxxxxxxxxxxxxx ",
  " x..................x ",
  " x.xxx.xxxxxxxx xxx.x ",
  " x.x......xx......x.x ",
  " x.x.xxx.xxxx.xxx.x.x ",
  " x..................x ",
  " x.XXX.XXX..XXX.XXX.x ",
  " x...X...X..X...X.X.x ",
  " x.XXX XXX..XXX XXX.x ",
  " x.X.....X....X...X.x ",
  " x.XXX XXX..XXX.XXX.x ",
  " x..................x ",
  " x.xx.x.xxxxxx.x.xx.x ",
  " x....x........x....x ",
  " xxxxxxxxxxxxxxxxxxxx ",
  "                      "


};

//char MAP[2][3] = { " x ", "  x"};
