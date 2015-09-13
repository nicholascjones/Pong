// Pong
// authors: Tallis Bowers, nicholascjones
// Interpretation of classic video game Pong.

//preprocessing

#include <stdio.h> 
#include <unistd.h> //animation
#include <math.h> //calculation
#include <stdlib.h> //random number functions
#include <string.h>
#include <time.h> //random number seeds
#include "gfx3.h" //graphics

#define XSIZE 1000 //x-dimension of screen
#define YSIZE 800  //y-dimension of screen
#define XC 500 //x-center of screen
#define YC 400 //y-center of screen
#define X1 0 //left x-coordinate of paddle 1 is always 0
#define X2 990 //x-coordinate of paddle 2 is always 990
#define WIDTH 10 //paddle width is 10
#define HEIGHT 100 //paddle width is 50
#define RADIUS 5 //ball radius is 5
#define STEP .05 //timestep is 1/10 of a  second 
#define PV 150.00 //paddle velocity is 5.00 pixels per STEP, or 50 pixels (one paddle) per second

int Initialize( float[3], float[3], float[2], int); //resets gameplay following the ending of a point, sets isOut to 0
	//parameters: x[],y[], v[]

void Recalculate( float*, float[3], float[2],int, char); //recalculates current positions recursively given current positions and velocity
	//parameters: x[0] (ball xposition), y[] (y positions of ball, both paddles), v[] (ball x and y velocities)

void Animate( float[3], float[3], int[3]); //animates the current positions of the objects and writes score
	//parameters: x[], y[], score[]

void Bounce(float[3], float[3], float[2]); //re-evaluates velocity in opposite direction
	//parameters: v[]....calls on value of xCheckBounce() and yCheckBounce() functions

int xCheckBounce( float[3], float[3]); //checks current position of ball against paddles
	//parameters: x[], y[]
	//outputs: arbitrary

int yCheckBounce( float[3]); //checks current position of ball against paddles and top and bottom walls 
        //parameters: x[], y[]
	//outputs: arbitrary

int CheckOut( float * ); //checks if ball is "out" on either side, and determines to whom the point goes
	//parameters: x[0]
	//ouputs: 0 = not out, 1 = point to p1, 2 = point to p2;

void UpdateScore( int, int[3]); //updates both scores
	//parameters: result of CheckOut function, either 0, 1, or 2.
	//outputs: score[0]++, score[winner]++

int GameOver( int[3]); //checks to see if either player has 11 points to win
	//parameters: scores[]

char *num2str(int); //converts number to string

//main function execution

int main()
{
	//declarations

	float x[3], y[3];
	int score[3]={0}; // x[3], y[3] are object locations, score[3] are scores
	int isOut = 0; //checks for ball being out and determines who gets the point
	int i; //counter
	float* xPtr;
	float v[2] = {0}; //ball velocities
	char c; //menu input, uses gfx_wait
	char Menu[4][50]; //displays strings in menu
	int q = 0;
	int b = 0;

	//initializing variable values
	x[1] = X1; //constant x location for paddle 1
	x[2] = X2; //constant x location for paddle 2		
	xPtr = &x[0]; //points to ball's xlocation

	strcpy(Menu[0],"Welcome to Pong!");
	strcpy(Menu[1],"To play an easy game .... Press e");
	strcpy(Menu[2],"To play a hard game ... Press h");
	strcpy(Menu[3],"To quit at any time ... Press q");
	
	//open window & initializes menu choices

	gfx_open(XSIZE,YSIZE,"Pong"); //opens window to start


	//menu
	for (i = 0; i < 4; i++) {
		//color
		switch (i) {
			case (1): gfx_color(255,255,255);
				  break;
			case (2): gfx_color(255,0,0);
				  break;
			case (3): gfx_color(0,255,0);
				  break;
			case (0): gfx_color(100,100,255);
				  break;
		}
		gfx_text(XSIZE/3,(i+1)*YSIZE/5,Menu[i]);
		gfx_flush();
	}
	c = gfx_wait(); //takes input
	if (c == 'q') return 0;

	// Game Control While Loop
	while ((c != 'q') && (GameOver(score) == 0)) { //game terminates each time 
		b++;
		q++;
		isOut = Initialize(x,y,v,q);
		Animate(x, y, score); //draws initial points
		//Point Control While Loop
		while (isOut == 0) { //each iteration is a timestep
			Recalculate( xPtr, y, v, b, c);
			Bounce( x, y, v);
			if (xCheckBounce(x,y) == 1) b++;
			isOut = CheckOut(xPtr); //checks if is out
                	Animate(x, y, score);			
			UpdateScore(isOut, score);
			GameOver( score);
		} //ends point
	} //ends game			

} //ends main function execution


//functions

//Initializes the important values after each point after each point
int Initialize( float x[3], float y[3], float v[2], int q)
{
	//declarations
	time_t t; //establishes time
	float r; // random number
	// float randy; //random float     ????
	int i; //counter
	int zero = 0;

	//position recalculations
	
	//ball position
	x[0] = XC;
	y[0] = YC;
	
	//paddle positions
	x[1] = X1;
	x[2] = X2;
	y[1] = YC;
	y[2] = YC;

	//random number calculation and velocity establishment
	
	for (i = 0; i <= 1; i++) {
		srand( (float)q*(float)time(&t) + (float)time(&t)*(float)i); //random seed for vx, differs for two directions
		r = (((float)rand()-(RAND_MAX))/(RAND_MAX/2)); //random proportion from 0 to 1
		v[i] = (r*PV)+150.00; //random proportion of 5.00

	} //ends execution of random loop

	return zero; //new value of isOver variable

} //ends Initialize function execution

//determines if the game is over given the score
int GameOver(int score[3])
{
	int w = 0; //winner value for game, default is 0
	int i; //counter

	for (i=1; i<=2; i++) {
		if (score[i] >= 11)
			w = i;
	} //ends for loop
	return w; //returns winner of game
}

//recalculates the positions for all of the relevant objects
void Recalculate(float* xPtr, float y[3], float v[2], int b, char lvl)
{
	char c;	
	float r;
	time_t t; //time

	//ball
	*xPtr = (float)*xPtr + v[0]*STEP;
	y[0] = (float)y[0] + v[1]*STEP;

	//paddle 1
	if(gfx_event_waiting())  {
		c = gfx_wait();
		if(c == 's')
			y[1]-=20;
		if(c  == 'x')
			y[1]+=20;  }
	
	//paddle 2
	switch (lvl) {
			case ('h'): srand( (float)time(&t)+5261.0*(float)b);
				    r = (rand()/(float)RAND_MAX)*100.00; //random value between 1 and 50	
				    y[2] = y[0]-50.00+r;
				    break;
			case ('e'): break;
	}

}

//draws stuff
void Animate(float x[3], float y[3], int score[3])
{
	gfx_clear();
	int i; //counter

	//background
	gfx_clear_color(128,255,0); //sets "grass"

	//center lines
	gfx_color(255,255,255);
	gfx_line(XSIZE/2,0,XSIZE/2,YSIZE);
	gfx_line(0,YSIZE/2,XSIZE,YSIZE/2);

	//ball
	for (i = 1; i <= RADIUS; i++) {
		gfx_color(0,0,0);
		gfx_circle(x[0],y[0],i);
	}

	//paddles
	for (i = 1; i <= 2; i++) {
		switch (i) {
			case (1): gfx_color(0,0,255);
				  break;
			case (2): gfx_color(255,0,0);
				  break;
		}

		gfx_fill_rectangle(x[i],y[i],WIDTH,HEIGHT);
	}

	//scores
	for (i = 1; i <=2; i++) {
                switch (i) {
                        case (1): gfx_color(0,0,255);
                                  break;
                        case (2): gfx_color(255,0,0);
                                  break;
                }
		gfx_text( i*XSIZE/3, 100, num2str(score[i]));
	}	
	gfx_flush();
	usleep(10000);

} //end function

void Bounce(float x[3], float y[3], float v[2])
{
	if((xCheckBounce(x, y) == 1) || (xCheckBounce(x,y) == 2))
		v[0] = -v[0];
	if(yCheckBounce(y) == 1)
		v[1] = -v[1];
}

int xCheckBounce(float x[3], float y[3])
{
	if(x[0] < x[1]+WIDTH+RADIUS+1) {
		if(y[0] > y[1] && y[0] < y[1]+HEIGHT)
			return(1);
	} else if (x[0] > x[2]-RADIUS-1) {
		if(y[0] > y[2] && y[0] < y[2]+HEIGHT)
			return(2);
	} else {return(0);}
}

int yCheckBounce(float y[3])
{
	if(y[0] < 5*RADIUS)
		return(1);
	else if(y[0] > YSIZE-(5*RADIUS))
		return(1);
	else return(0);
}

//checks if ball is out
int CheckOut(float* xPtr)
{
	int w; //gives to winner
	if (*xPtr <= 0) {
		w = 2;
	} else if (*xPtr >= XSIZE) {
		w = 1;
	} else {w = 0;}

	return w;
}

//updates scores
void UpdateScore(int w, int score[3])
{
	if ((w == 1) || (w==2)) {
		score[0]++;
		score[w]++;
	}
}
 
char *num2str(int n)
{
  static char a[4]; 
  sprintf(a, "%d", n);
  return (char *) a;
}

