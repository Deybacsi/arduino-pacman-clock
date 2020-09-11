
#include <SPI.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
#include "constants.h"
#include "digits.h"
#include "myimgs.h"

MCUFRIEND_kbv tft;

PROGMEM const uint16_t ID = 0x6809;
//PROGMEM const uint16_t ID = 0x9341;

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define LGRAY   0xC618
#define LLGRAY  0xE71C
#define ORANGE  0xFDCA

unsigned long   timerStart;   // for counting FPS
#define FRAMEMS 100            // time to draw 1 frame -> 1000 ms / 20 FPS = 50
int16_t         forDelay;     // calculated ms to delay after each frame creation



void setup() {
  uint16_t i, j;
  randomSeed(analogRead(0));
  Serial.begin(9600);
  tft.reset();

  tft.begin(ID);

  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(BLACK);
  printScene();



  ghosts[0].coords.x = 2;
  ghosts[0].coords.y = 2;
  ghosts[0].color = RED;
  ghosts[0].direction = rand() % 2 + 1;

  ghosts[1].coords.x = SCRWIDTH - 1 ;
  ghosts[1].coords.y = 2;
  ghosts[1].color = CYAN;
  ghosts[1].direction = rand() % 2 + 2;


  ghosts[2].coords.x = 2;
  ghosts[2].coords.y = SCRHEIGHT - 1;
  ghosts[2].color = MAGENTA;
  ghosts[2].direction = rand() % 2;


  ghosts[3].coords.x = SCRWIDTH - 1 ;
  ghosts[3].coords.y = SCRHEIGHT - 1;
  ghosts[3].color = ORANGE;
  ghosts[3].direction = (rand() % 2) ? 0 : 3;

  for (i = 0; i < 4; i++) {
    ghosts[i].type = 1;
    ghosts[i].stepsCounter = 0;
    ghosts[i].lastcoords.x = (ghosts[i].coords.x - 1) * SPRITE_WIDTH;
    ghosts[i].lastcoords.y = (ghosts[i].coords.y - 1) * SPRITE_HEIGHT;
  }

  pacman.type = 0;
  pacman.color = YELLOW;
  pacman.coords.x = rand() % 2 + 10 ;
  pacman.coords.y = rand() % 5 + 7;
  pacman.lastcoords.x = (pacman.coords.x - 1) * SPRITE_WIDTH;
  pacman.lastcoords.y = (pacman.coords.y - 1) * SPRITE_HEIGHT;

  for (i = 0; i < 4; i++) {
    Serial.print("i: "); Serial.print(PACFRAMES[i][i]);
    Serial.print('\n');
  }

}

void loop() {

  // put your main code here, to run repeatedly:
  uint16_t i, j, tempX, tempY;

  //uint8_t mapCoordX, mapCoordY;
  uint8_t tempDirection, freeDirectionBits;
  bool dirWalkable, isDeadEnd;


  while (true) {
    timerStart = millis();
    pacman.stepsCounter += 2;

    tempX = (pacman.coords.x - 1) * SPRITE_WIDTH + pacman.stepsCounter * DIRMATRIX[pacman.direction].x;
    tempY = (pacman.coords.y - 1) * SPRITE_HEIGHT + pacman.stepsCounter * DIRMATRIX[pacman.direction].y;
    tft.drawBitmap(pacman.lastcoords.x, pacman.lastcoords.y , PACDELETE[pacman.direction], SPRITE_WIDTH, SPRITE_HEIGHT, BLACK);
    tft.drawBitmap(tempX, tempY, PAC[ PACFRAMES[pacman.direction][ (pacman.stepsCounter >> 1) % 4 ] ], SPRITE_WIDTH, SPRITE_HEIGHT,  pacman.color);


    if (pacman.stepsCounter >= SPRITE_WIDTH)  {
      pacman.stepsCounter = 0;

      pacman.coords.x += DIRMATRIX[pacman.direction].x ;
      pacman.coords.y += DIRMATRIX[pacman.direction].y ;

      MAP[pacman.coords.y][pacman.coords.x]=' ';
      
      pacman.lastdirection = (pacman.direction + 2) % 4;

      dirWalkable = false;
      isDeadEnd = false;
      freeDirectionBits = getSurroundingWalls(pacman.coords.x, pacman.coords.y);

      if (freeDirectionBits == 19 || freeDirectionBits == 11 || freeDirectionBits == 25 || freeDirectionBits == 26) {
        isDeadEnd = true;
      }

      while (!dirWalkable) {
        tempDirection = rand() % 4;
        dirWalkable = true;
        // if there is a wall in the direction
        if (MAP[pacman.coords.y + DIRMATRIX[tempDirection].y][pacman.coords.x + DIRMATRIX[tempDirection].x] == 'x'
            || MAP[pacman.coords.y + DIRMATRIX[tempDirection].y][pacman.coords.x + DIRMATRIX[tempDirection].x] == 'X') {
          dirWalkable = false;
        }

        if (tempDirection == pacman.lastdirection && !isDeadEnd )  {
          dirWalkable = false;
        }

      }

      pacman.direction = tempDirection;
      pacman.lastFreeDirectionBits = freeDirectionBits;

    }

    pacman.lastcoords.x = tempX;
    pacman.lastcoords.y = tempY;








    for (i = 0; i < 4; i++) {
      ghosts[i].stepsCounter += 2;

      // if there is a point below the ghost, then draw it
      if (MAP[ghosts[i].coords.y][ghosts[i].coords.x] == '.' ) {
        tft.drawBitmap((ghosts[i].coords.x - 1) * SPRITE_WIDTH, (ghosts[i].coords.y - 1) * SPRITE_HEIGHT, WALL[15], SPRITE_WIDTH, SPRITE_HEIGHT,  WHITE );
      }

      tempX = (ghosts[i].coords.x - 1) * SPRITE_WIDTH + ghosts[i].stepsCounter * DIRMATRIX[ghosts[i].direction].x;
      tempY = (ghosts[i].coords.y - 1) * SPRITE_HEIGHT + ghosts[i].stepsCounter * DIRMATRIX[ghosts[i].direction].y;
      tft.drawBitmap(ghosts[i].lastcoords.x, ghosts[i].lastcoords.y , GHOSTDELETE[ghosts[i].direction], SPRITE_WIDTH, SPRITE_HEIGHT, BLACK);
      tft.drawBitmap(tempX, tempY, GHOST[(ghosts[i].stepsCounter >> 2) % 2 ], SPRITE_WIDTH, SPRITE_HEIGHT,  ghosts[i].color);
      tft.drawBitmap(tempX, tempY + 3, EYES, SPRITE_WIDTH, SPRITE_EYES_HEIGHT, WHITE);
      tft.drawBitmap(tempX + DIRMATRIX[ghosts[i].direction].x * 2, tempY + DIRMATRIX[ghosts[i].direction].y * 2 + 4, PUPIL, SPRITE_WIDTH, SPRITE_PUPIL_HEIGHT, BLUE);


      if (ghosts[i].stepsCounter >= SPRITE_WIDTH)  {
        ghosts[i].stepsCounter = 0;


        ghosts[i].coords.x += DIRMATRIX[ghosts[i].direction].x ;
        ghosts[i].coords.y += DIRMATRIX[ghosts[i].direction].y ;
        ghosts[i].lastdirection = (ghosts[i].direction + 2) % 4;

        dirWalkable = false;
        isDeadEnd = false;
        freeDirectionBits = getSurroundingWalls(ghosts[i].coords.x, ghosts[i].coords.y);

        if (freeDirectionBits == 19 || freeDirectionBits == 11 || freeDirectionBits == 25 || freeDirectionBits == 26) {
          isDeadEnd = true;
        }

        while (!dirWalkable) {
          tempDirection = rand() % 4;
          dirWalkable = true;
          // if there is a wall in the direction
          if (MAP[ghosts[i].coords.y + DIRMATRIX[tempDirection].y][ghosts[i].coords.x + DIRMATRIX[tempDirection].x] == 'x'
              || MAP[ghosts[i].coords.y + DIRMATRIX[tempDirection].y][ghosts[i].coords.x + DIRMATRIX[tempDirection].x] == 'X') {
            dirWalkable = false;
          }

          if (tempDirection == ghosts[i].lastdirection && !isDeadEnd )  {
            dirWalkable = false;
          }

        }

        ghosts[i].direction = tempDirection;
        ghosts[i].lastFreeDirectionBits = freeDirectionBits;

      }

      ghosts[i].lastcoords.x = tempX;
      ghosts[i].lastcoords.y = tempY;
    }


    
        Serial.print("Frame: "); Serial.print(millis() - timerStart);
        Serial.print("Delay: "); Serial.print(forDelay);
        Serial.print('\n');
    

    forDelay = FRAMEMS - (millis() - timerStart);

    (forDelay > 0) ? delay(forDelay) : delay(0);




  }




}



void printChar(uint8_t x, uint8_t y, uint8_t ch) {
  tft.setCursor(x * CHRWIDTH, y * CHRHEIGHT);
  tft.print(char(ch));
}

// get the actual x,y position and the surrounding up, left, right, down tile
// and make a bitmask from them to see where are walls
// 1 means wall, 0 - walkable
// for eg:
//  0
// 111
//  0
// -> 01110 = 21 -> it will be a horizontal line -> wall sprite 1

uint8_t getSurroundingWalls(uint8_t x, uint8_t y) {
  uint8_t tempBits = 0;
  (MAP[y - 1][x] == 'x' || MAP[y - 1][x] == 'X') ? tempBits |= 1 << 4 : tempBits &= ~(1 << 4) ;
  (MAP[y][x - 1] == 'x' || MAP[y][x - 1] == 'X') ? tempBits |= 1 << 3 : tempBits &= ~(1 << 3) ;
  (MAP[y][x]     == 'x' || MAP[y][x] == 'X')     ? tempBits |= 1 << 2 : tempBits &= ~(1 << 2) ;
  (MAP[y][x + 1] == 'x' || MAP[y][x + 1] == 'X') ? tempBits |= 1 << 1 : tempBits &= ~(1 << 1) ;
  (MAP[y + 1][x] == 'x' || MAP[y + 1][x] == 'X') ? tempBits |= 1 << 0 : tempBits &= ~(1 << 0) ;
  return tempBits;
}

void printScene() {
  uint8_t i, j = 0;
  tft.setTextColor(BLUE);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);

  // print out the scene

  char tempWalls, tempBits;
  uint8_t printSpriteNo = 0;
  uint16_t  tempColor;

  for (j = 1; j <= SCRHEIGHT + 1; j++) {
    for (i = 1; i < SCRWIDTH + 1; i++) {
      printSpriteNo = 0;
      tempBits = 0;

      // X = white , x = blue wwall
      if (MAP[j][i] == 'X') {
        tempColor = WHITE;
      } else {
        tempColor = BLUE;
      }

      tempBits = getSurroundingWalls(i, j);

      switch (tempBits) {
        case 21 : printSpriteNo = 0;  // vertical line
          break;
        case 14 : printSpriteNo = 1;  // horitontal line
          break;
        case 7 : printSpriteNo = 2;  // top left
          break;
        case 13 : printSpriteNo = 3;  // top right
          break;
        case 28 : printSpriteNo = 4;  // bottom right
          break;
        case 22 : printSpriteNo = 5;  // bottom left
          break;
        case 15 : printSpriteNo = 6;  // T down
          break;
        case 29 : printSpriteNo = 7;  // T left
          break;
        case 30 : printSpriteNo = 8;  // T up
          break;
        case 23 : printSpriteNo = 9;  // T right
          break;
        case 20 : printSpriteNo = 10;  // end bottom
          break;
        case 6 : printSpriteNo = 11;  // end left
          break;
        case 5 : printSpriteNo = 12;  // end up
          break;
        case 12 : printSpriteNo = 13;  // end right
          break;
        case 4 : printSpriteNo = 14;  // small circle
          break;
        default : printSpriteNo = 15; tempColor = WHITE;  // point
          break;
      }



      //tft.drawBitmap((i - 1)*SPRITE_WIDTH, (j - 1)*SPRITE_HEIGHT, CLEARSPRITE[0], SPRITE_HEIGHT, SPRITE_WIDTH, BLACK);
      tft.drawBitmap((i - 1)*SPRITE_WIDTH, (j - 1)*SPRITE_HEIGHT, WALL[printSpriteNo], SPRITE_HEIGHT, SPRITE_WIDTH, tempColor);

    }
  }



}
