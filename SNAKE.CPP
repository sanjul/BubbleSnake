/*  31/1/2011 2:25 AM
   (c) Sanjul.A.S
   MIT Licence (http://opensource.org/licenses/MIT)
   --------------------------------
   use W,A,S,D for up,left,down,right
   q- to quit
   */
#include<iostream.h>
#include<string.h>
#include<stdlib.h>
#include<conio.h>
#include<dos.h>
#include<process.h>
#include<graphics.h>
#define MAXLENGTH 25
#define BLOCKSIZE 15
#define BLOCKRAD BLOCKSIZE/2
#define LEFT 10
#define TOP 10
#define WIDTH 500
#define HEIGHT 400
#define SNAKE_COLOR RED
#define FOOD_COLOR BLUE
#define MAXDELAY 100
int DELAY=MAXDELAY;
int level=1;
int reset=1;
class snake;
class gui;
enum direction { left,right,up,down,none};

direction opposite(direction dir)
 {
  switch(dir)
   {
    case left: return(right);
    case right: return(left);
    case up:return(down);
    case down:return(up);
    default:
     return(none);
   }
 }
int isinrange(int v1,int v2,int rad)
 {
   int r=0;
   int v1min,v1max,v2min,v2max;
   v1min=(int)v1-rad*1.5;
   v1max=(int)v1+rad*1.5;
   v2min=(int)v2-rad*1.5;
   v2max=(int)v2+rad*1.5;
   if((v2>=v1min&&v2<=v1max)||(v1>=v2min&&v1<=v2max))
     r=1;
   return(r);

 }
void paintintxy(int x,int y,int val)
 {
  char str[25];
  itoa(val,str,10);
  outtextxy(x,y,str);
 }
int countchar(char *str,char ch)
 {
  int count=0;
  for(int i=0;i<strlen(str);i++)
    {
      if(str[i]==ch)
	count++;
    }
  return(count);
 }
class gui
 {
  public:
   int line;
   gui(){ line=0;}
   void print(char *msg)
    {
      outtextxy((getmaxx()/2)-10*(strlen(msg)/2),(getmaxy()/2)-line*10,msg);
      line++;
    }
 }form;
direction getdirection(char key)
 {
    direction tmp;
    switch(key)
	 {
	  case 'a': tmp=left; break;
	  case 'd': tmp=right; break;
	  case 's': tmp=down;break;
	  case 'w': tmp=up;break;
	  case 'q': form.print("Are you sure want to exit?(y/n)");
		    if(getche()=='y')
		    exit(0);
	  default:
	    tmp=none;
	 }
	 return(tmp);
 }

class point
 {
  public:
  int x,y;
  direction dir;
 };
class viewport
 {
  public:
  int left,top,width,height;
  int wrapx(int val)
  {
   if(val>width)
    {
     val=val%width;
     if(val==0) val=width;
     return(val);
    }
   else if(val<left)
     val=width-abs(val)-left;
    return val;
  }
  int wrapy(int val)
  {
   if(val>height)
    {
     val=val%height;
     if(val==0) val=height;
     return(val);
    }
   else if(val<top)
     val=height-abs(val)-top;
    return val;
  }
  viewport(){
    left=LEFT;
    top=TOP;
    width=WIDTH;
    height=HEIGHT;
  }
  void drawedges()
   {
    rectangle(left,top,left+width,top+height);
   }
  void drawcircle(int x,int y,int rad)
   {
    int cx,cy;
    cx=left+wrapx(x);
    cy=top+wrapy(y);
    circle(cx,cy,rad);
    floodfill(cx,cy,WHITE);

   }
   void clear();
 }vp;
void viewport::clear()
 {
  cleardevice();
 }
point getVector(direction dir)
 {
  point r;
  r.dir=dir;
  switch(dir)
   {
    case left: r.x=-BLOCKSIZE;
	      r.y=0; break;
    case right: r.x=BLOCKSIZE;
	      r.y=0; break;
    case up: r.x=0;
	      r.y=-BLOCKSIZE; break;
    case down: r.x=0;
	      r.y=BLOCKSIZE; break;
    default:
	    r.x=r.y=0;
	    r.dir=none;
   }
  return(r);
 }
enum snakestate
 {
   normal,eat,eatself,hitmaze
 };
class snake
 {
  public:
  point body[MAXLENGTH+1];
  int length,score;
  point food;
  snakestate status;
  void init() {
    length=0;
    status=normal;
    body[0].dir=right;
    food.dir=none;
    body[0].x=getmaxx()/2;
    body[0].y=getmaxy()/2;
    grow();
    grow();
    grow();
    }
  void reSet()
   {
    init();
    score=0;
    reset=0;
    level=0;
    DELAY=MAXDELAY;
   }
  snake()
   {
    reSet();
   };
  void move(direction);
  void grow();
  void render();
  void hittest();
  void printstatus();
  void generatefood()
   {
     food.x=1+rand()%(vp.width-1);
     food.y=1+rand()%(vp.height-1);
     food.dir=none;
   }
 }s;
void snake::printstatus()
   {
     switch(status)
      {
       case eatself: outtextxy(10,10,"Eating itself");
		 break;
       case eat: outtextxy(10,10,"Eating");break;
       case hitmaze: outtextxy(10,10,"snake hit the maze");break;
      }
   }
void snake::hittest()
 {
   int hx=body[0].x,hy=body[0].y;
   int cx,cy;
   for(int i=1;i<=length;i++)
    {
      int wfx,wfy,wcx,wcy;
      cx=body[i].x;
      cy=body[i].y;
      if(cx==hx&&cy==hy)
	status=eatself;
      wcx=vp.wrapx(cx);
      wcy=vp.wrapy(cy);
      wfx=vp.wrapx(food.x);
      wfy=vp.wrapy(food.y);
      if(isinrange(wfx,wcx,BLOCKSIZE/2)&&isinrange(wfy,wcy,BLOCKSIZE/2))
	{
	 grow();
	 score++;
	 generatefood();
	}

    }
 }
void snake::move(direction dir)
 {
   point temp,prev;
   if(reset==1)
     reSet();
   if(length>=MAXLENGTH)
    {
     form.print("LEVEL COMPLETED!! PRESS ANY KEY TO CONTINUE");
     level++;
     getch();
     init();
     DELAY-=5;
     if(DELAY<5)
      {
       form.print("GAME COMPLETED!!");
       form.print("");
       gamecomplete:
       if(getche()==' ')
	reSet();
       else
	goto gamecomplete;
      }
    }
   body[0].x=vp.wrapx(body[0].x);
   body[0].y=vp.wrapy(body[0].y);
   prev=body[0];
   if(opposite(dir)==prev.dir||dir==none)
     dir=prev.dir;
   temp=getVector(dir);
   body[0].x+=temp.x;
   body[0].y+=temp.y;
   body[0].dir=dir;
   for(int i=1;i<=length;i++)
    {
      temp=body[i];
      body[i]=prev;
      prev=temp;
    }
   hittest();
 }
void snake::grow()
 {
   length++;
   point temp;
   temp=getVector(body[length-1].dir);
   body[length]=body[length-1];
   body[length].x-=temp.x;
   body[length].y-=temp.y;
 }
void snake::render()
 {
  //cleardevice();
  vp.clear();
  /*paintintxy(100,100,body[0].x);
  paintintxy(100,110,body[0].y);
  paintintxy(300,100,food.x);
  paintintxy(300,110,food.y);*/
  setfillstyle(SOLID_FILL,FOOD_COLOR);
  vp.drawcircle(food.x,food.y,BLOCKSIZE/2);
  outtextxy(110,2,"Score:");
  paintintxy(160,2,score);
  outtextxy(200,2,"Level:");
  paintintxy(250,2,level);
  setfillstyle(SOLID_FILL,SNAKE_COLOR);
  for(int i=0;i<=length;i++)
   {
    vp.drawcircle(body[i].x,body[i].y,BLOCKSIZE/2);
   }
   vp.drawedges();
 }

void main()
 {
   char key;
   direction dir=right;
   int gm,gd=DETECT;
   initgraph(&gd,&gm,"c:\\tctemp");
   s.render();
   s.generatefood();
   while(1)
    {
     if(kbhit())
      {
	key=getche();
	dir=getdirection(key);
      }
     delay(DELAY);
     s.move(dir);
     s.render();
     //s.printstatus();
     if(s.status==eatself)
       {

       form.print("GAME OVER...! PRESS SPACE TO CONTINUE");
       gameover:
       if(getche()==' ')
	s.reSet();
       else
	goto gameover;
       }
     else if(s.status==eat)
      {
       s.score++;
       s.generatefood();
      }
    }
 }