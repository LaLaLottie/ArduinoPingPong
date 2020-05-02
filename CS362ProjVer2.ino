/* Ping Pong Game using an Arduino. */
/* Name: Charlotte Norman, NetID: cnorma4 */
/* Name: Binderiya Bolor, NetID: bbolor2 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Initializations

// TFT DISPLAY
#define TFT_CS 10
#define TFT_RST 6
#define TFT_DC 8
#define TFT_SCLK 13
#define TFT_MOSI 11
Adafruit_ST7735 screen = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const int joystick_ypin = A0;
const int button = 7;

int currState;

struct Player {
  int yPos; // Initial Y Position
  int xPos; // X position that will remain unchanged, but varies between player and computer.
  int score;

  void draw() {
    screen.drawLine(xPos, yPos, xPos, yPos + paddleLen, ST7735_WHITE);
  }
  
  void erase() { // Function that 'erases' the paddle so that it can be redrawn.
    screen.drawLine(xPos, 10, xPos, screen.height(), ST7735_BLACK);
  }
};

struct Ball {
  int yPos;
  int xPos;
  int radius;

  void draw() {
    screen.fillCircle(xPos, yPos, radius, ST7735_RED);
  }

  void erase() {
    screen.fillCircle(xPos, yPos, radius, ST7735_BLACK);
  }
};

int screen_width = 159; // 0 - 159 ~ 160
int screen_height = 127; // 10 - 127 ~ 118
int half_width = screen_width / 2;
int half_height = screen_height / 2;
int paddleLen = 16;

int xBallIncre = -1; // Incrementator for X used to change ball's X.
int yBallIncre = 0; // Incrementator for Y used to change ball's Y.
  
Player user; 
Player comp;
Ball ball;

void setup() {
  Serial.begin(9600);
  
  // Initializer for tft display **** ONLY FOR OUTPUT ARDUINO, BUT LEFT HERE FOR TESTING
  screen.initR(INITR_BLACKTAB); 
  
  // Set to landscape mode.
  screen.fillScreen(ST7735_BLACK);  
  screen.setRotation(3); // NOTE: MAY HAVE TO CHANGE TO 3 DEPENDING ON FINAL SETUP
  
  // Set button as input.
  pinMode(button, INPUT);

  // Initialize paddles and ball.
  user.yPos = comp.yPos = (half_height + 10) - (paddleLen / 2);
  user.xPos = 1;
  comp.xPos = screen_width - 2;
  user.score = comp.score = 0;  
  
  ball.xPos = half_width;
  ball.yPos = half_height + 10;
  ball.radius = 2;

  currState = -1;
}

void displayGame() {
  // Clear the screen.
  screen.fillScreen(ST7735_BLACK);

  scoreBanner();
  user.draw();
  comp.draw();
  ball.draw();
  
  // STILL NEEDS: after erasure of the computer paddle 
  // and ball, figure out the next position and then draw new
  
  while (digitalRead(button) == LOW) {
    moveBallPosition();
    readUserMove();
    moveComputerPosition();
  }
  
  /*// PLACEHOLDER
  screen.setCursor(half_width - 60, half_height);
  screen.setTextColor(ST7735_CYAN, ST7735_BLACK);
  screen.println("IN GAME DISPLAY"); 
  */
}

void endGame() {
  String win = "YOU WIN!";
  String lose = "YOU LOSE!";
  String tie = "IT'S A TIE!";
  String cont = "End Game";

  // clear the screen
  screen.fillScreen(ST7735_BLACK);

  // determine outcome
  if (user.score > comp.score) {
    // user wins
    center(win, 2, half_height);
    screen.setTextColor(ST7735_CYAN);
    screen.print(win);
  }
  else if (user.score < comp.score) {
    // user loses
    center(lose, 2, half_height);
    screen.setTextColor(ST7735_RED);
    screen.print(lose);
  }
  else {
    // tie
    center(tie, 2, half_height);
    screen.setTextColor(ST7735_GREEN);
    screen.print(tie);
  }

  // TODO: SHOW SCORES

  // direct user to exit to menu
  center(cont, 1, screen.height() - 10);
  screen.setTextColor(ST7735_BLACK, ST7735_WHITE);
  screen.print(cont);
  while(digitalRead(button) == LOW) {
      
   }  
}

void center(String text, int textSize, int height) {
  int offset = textSize * 6 * (text.length() / 2);
  screen.setTextSize(textSize); 
  screen.setCursor(half_width - offset, height);
}

void scoreBanner() {
  String player = "Player";
  String computer = "Computer";
  String currScores = String(user.score) + " : " + String(comp.score);

  // display user's side
  screen.setCursor(0, 0);
  screen.setTextSize(1);
  screen.setTextColor(ST7735_BLUE);
  screen.print(player);

  // display scores: invert colors (once - can be changed easily) to show the change
  center(currScores, 1, 0);
  screen.setTextColor(ST7735_BLACK, ST7735_WHITE);
  screen.print(currScores);
  center(currScores, 1, 0);
  screen.setTextColor(ST7735_WHITE, ST7735_BLACK);
  screen.print(currScores);  

  // display computer's side
  screen.setCursor(screen.width() - (6 * computer.length()), 0);
  screen.setTextColor(ST7735_RED);
  screen.print(computer);

  // display lower border
  screen.drawLine(0, 9, screen.width(), 9, ST7735_WHITE);
}

void readUserMove() {
  int newY = analogRead(joystick_ypin);

  /*
   * 0 - 1024
   * no movement: will read ~ 512
   * 
   * 512 + 64 = 576
   * 512 - 64 = 448
   * 
   * 913 - 1023: -4
   * 801 - 912: -3
   * 689 - 800: -2
   * 576 - 688: -1  
   * 448 - 575: no movement
   * 336 - 447: +1
   * 224 - 335: +2
   * 112 - 223: +3
   * 0 - 111: +4
   */
   
   if (newY < 112) {
    user.yPos -= 4;
   }
   else if (newY < 224) {
    user.yPos -= 3;
   }
   else if (newY < 336) {
    user.yPos -= 2;
   }
   else if (newY < 448) {
    user.yPos -= 1;
   }
   else if (newY < 576) {
    return;
   }
   else if (newY < 688) {
    user.yPos += 1;
   }
   else if (newY < 800) {
    user.yPos += 2;
   }
   else if (newY < 912) {
    user.yPos += 3;
   }
   else if (newY < 1024) {
    user.yPos += 4;
   }

  if (user.yPos >= screen.height() - paddleLen) {
    user.yPos = screen.height() - paddleLen;
  } else if (user.yPos <= 10) {
    user.yPos = 10;
  }
   
   user.erase();
   user.draw();
   delay(50);
}

void gameReset() {
  user.score = comp.score = 0;
  user.yPos = comp.yPos = (screen.height() - 10) / 2;
  user.xPos = 1;
  comp.xPos = screen.width() - 2;

  ball.xPos = half_width;
  ball.yPos = (screen.height() - 10) / 2;
  ball.radius = 2;
}

void moveComputerPosition() {
  int toWin; // Will determine if the computer will win this round or not.
  
  int ballPaddleDist = 0; // Y displacement between the ball and the computer paddle.
  int halfwayCompWidth = half_width + (half_width / 2); // Halfway mark on computer side.
  int dir; // Will generate a random number 0 or 1 which will determine if the paddle goes up or down. 0 - Down, 1 - Up
  
  srand(time(NULL));

  //int r = (rand() % 3) + 1; // Will generate a number between 1 - 4 to use for random paddle displacement.
  
  dir = rand() % 2; 
  toWin = rand() % 2;
    
  //if (ball.xPos >= half_width) { // Ball position after it crosses the halfway mark to be on the computer's side, paddle just matches the ball's y position.
  comp.yPos = ball.yPos; 
  //}
  
  if (ball.xPos >= halfwayCompWidth) { // Ball position right when it's at or crosses the halfway mark; will either hit or miss the paddle depending on if the computer should win or not.
    ballPaddleDist = ball.yPos - comp.yPos;

    // Uses the difference in Y between the ball and paddle to move the paddle enough so the ball doesn't cross.  
    // Makes sure the paddle doesn't move too much that it misses the ball on the opposite end.

    if (toWin) { // Computer paddle hits/wins.
      if (comp.yPos == 10) { // Paddle is at maximum height.
        comp.yPos = comp.yPos + ballPaddleDist + paddleLen + 1;
      }
      else if (comp.yPos = (screenHeight - paddleLen)) { // Paddle is at minimum height.
        comp.yPos = comp.yPos - ballPaddleDist - paddleLen - 1;
      }
      else if ((ballPaddleDist < 0) && ((-ballPaddleDist) < paddleLen)) { // Ball is Lower, Within Paddle Length
        comp.yPos = ballPaddleDist + 1;
      }
      else if ((ballPaddleDist < 0) && ((-ballPaddleDist) > paddleLen)) { // Ball is Lower
        comp.yPos = ballPaddleDist + paddleLen + 1;
      }
      else if ((ballPaddleDist > 0) && (ballPaddleDist < paddleLen)) { // Ball is Higher, Within Paddle Length
        comp.yPos = ballPaddleDist - 1;
      }
      else if ((ballPaddleDist > 0) && (ballPaddleDist > paddleLen)) { // Ball is Higher
        comp.yPos = ballPaddleDist - paddleLen - 1;
      }
    }
    else { // Computer paddle misses/loses.
      if (comp.yPos == 10) { // Paddle is at maximum height.
        comp.yPos = comp.yPos - ballPaddleDist - paddleLen - 1;
      }
      else if (comp.yPos = (screenHeight - paddleLen)) { // Paddle is at minimum height.
        comp.yPos = comp.yPos + ballPaddleDist + paddleLen + 1;
      }
      else if ((ballPaddleDist < 0) && ((-ballPaddleDist) < paddleLen)) { // Ball is Lower, Within Paddle Length
        comp.yPos = ballPaddleDist - 1;
      }
      else if ((ballPaddleDist < 0) && ((-ballPaddleDist) > paddleLen)) { // Ball is Lower
        comp.yPos = ballPaddleDist - paddleLen - 1;
      }
      else if ((ballPaddleDist > 0) && (ballPaddleDist < paddleLen)) { // Ball is Higher, Within Paddle Length
        comp.yPos = ballPaddleDist + 1;
      }
      else if ((ballPaddleDist > 0) && (ballPaddleDist > paddleLen)) { // Ball is Higher
        comp.yPos = ballPaddleDist + paddleLen + 1;
      }
    }
  }
  else { // Ball is on the user's side, the computer paddle just moves randomly.
    if (r == 1) { // Paddles goes up.
      if (comp.yPos > 10) { 
        comp.yPos -= 1;
      }
      else { // Upper Height Limit, right?
        comp.yPos = 10;
      }
    }
    else { // Paddle goes down.
      if ((comp.yPos + paddleLen) < (screen_height - paddleLen)) { 
        comp.yPos += 1;
      }
      else { // Lower Height Limit
        comp.yPos = screen_height - paddleLen;
      }
    }
  }
}

//void incrementBall() {
  // check if ball has hit paddle
    // if so, negate the x increment
    // and update the y increment based on where it hits the paddle
  // check if ball has hit border
    // if so, negate the y increment

  //ball.xPos += ballXinc;
  //ball.yPos += ballYinc;
//}

void changeBallYIncrement(int region) {
  if (yBallIncre > 0) { // Ball is coming in downwards...
    if (region > 0) { // Top Paddle Regions
      yBallIncre += region;
    }
    else if (region == 0) { // Middle Paddle Region
      yBallIncre += region;
    }
    else if (region < 0) { // Bottom Paddle Regions
      yBallIncre = -yBallIncre;
    }
  }
  else if (yBallIncre < 0) { // Ball is coming in upwards...
    if (region > 0) { // Top Paddle Regions
      yBallIncre = -yBallIncre;
    }
    else if (region == 0) { // Middle Paddle Region
      // Do Nothing
    }
    else if (region < 0) { // Bottom Paddle Regions
      // Do Nothing
    } 
  }
  else if (yBallIncre == 0) { // Ball is coming in straight...
    yBallIncre += region; // Changing Y increment value based on paddle region hit.
  }
}

void changeBallYDirection(Player p) {
  int region = 0;
  
  if ((ball.yPos >= p.yPos) && (ball.yPos <= (p.yPos + 5))) { // 0 - 3
    region = 2;
    changeBallYIncrement(region);
  }
  else if ((ball.yPos >= (p.yPos + 4)) && (ball.yPos <= (p.yPos + 7))) { // 4 - 7
    region = 1;
    changeBallYIncrement(region);
  }
  else if ((ball.yPos >= (p.yPos + 8)) && (ball.yPos <= (p.yPos + 9))) { // 8 - 9
    region = 0;
    changeBallYIncrement(region);
  }
  else if ((ball.yPos >= (p.yPos + 10)) && (ball.yPos <= (p.yPos + 13))) { // 10 - 13
    region = -1;
    changeBallYIncrement(region);
  }
  else if ((ball.yPos >= (p.yPos + 14)) && (ball.yPos <= (p.yPos + 16))) { // 14 - 16
    region = -2;
    changeBallYIncrement(region);
  }
}

void checkIfBallHitsPaddle() {
  if (ball.xPos == 0 && (ball.yPos >= user.yPos && ball.yPos <= (user.yPos + paddleLen))) { // Hits User's Paddle
    xBallIncre = -xBallIncre;
    changeBallYDirection(user);
  }
  else if (ball.xPos == 159 && (ball.yPos >= user.yPos && ball.yPos <= (user.yPos + paddleLen))) { // Hits Computer's Paddle
    xBallIncre = -xBallIncre;
    changeBallYDirection(comp);
  }
}

void checkIfBallHitsBorders() {
  if (ball.yPos == 10 || ball.yPos == 159) { // If ball touches the top or bottom border, then just change the Y direction.
    yBallIncre = -yBallIncre;
  }

  // Should checking the right and left borders be done elsewhere? 
}

void moveBallPosition() {
  //srand(time(NULL);
  //int diagonalY = ball.yPos - 1;
  //int r = (rand() % 3) + diagonalY; // Randomly decide to go straight, diagonally up, or diagonally down.
  
  //if ((ball.xPos == half_width) && (ball.yPos == (half_height + 10))) { // Ball is in starting position.
    //ball.xPos = ball.xPos - xIncre; // Always goes to user's side first.
  //}
  
  checkIfBallHitsPaddle(); 
  
  checkIfBallHitsBorders();
  
  ball.xPos += xBallIncre;
  ball.yPos += yBallIncre;
  
  // sendSerial();
}

void loop() {
  displayGame();
  //endGame();
  gameReset();
  
  delay(1000);
}
