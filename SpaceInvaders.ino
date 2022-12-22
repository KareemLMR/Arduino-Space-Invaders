#include <Adafruit_PCD8544.h>
#include <timer.h>

auto timer = timer_create_default(); // create a timer with default settings
auto timer2 = timer_create_default(); // create a timer with default settings
auto timer3 = timer_create_default(); // create a timer with default settings

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 8);

typedef enum{Stop,Right,Left}motion_t;
typedef enum{HOLD,FIRE}gun_t;
gun_t gun = HOLD;
motion_t motion = Stop;
char motionPtr = 0;

int rear = 0;
int front = 0;

int enemy_rear = 0;
int enemy_front = 0;

const unsigned char Rocket [] PROGMEM = {0x80, 0x00, 0x10, 0x80, 0x40, 0x10, 0x80, 0x60, 0x10, 0x80, 0x60, 0x10, 0x80, 0x60, 0x10, 0x80, 
0xf4, 0x10, 0x83, 0xfc, 0x90, 0x9f, 0xfe, 0x90, 0x80, 0x40, 0x10, 0x80, 0x00, 0x10
};

const unsigned char bullet [] PROGMEM = {0x00, 0x60, 0x60, 0x00, 0x00
};

const unsigned char enemy[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x01, 0x68, 0x00, 0x00, 
0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

signed char enemy_ships[] = {2 , 50 , 60};

typedef struct bullet_t
{
  int x;
  int y;
}bullets;
bullets bullet_process[100]; 
bullets enemy_bullets[100]; 

void chooseMovement1()
{
  if(digitalRead(2))
  {
    motion = Right;
  }
  else
  {
    motion = Stop;
  }
}
void chooseMovement2()
{
  if(digitalRead(3))
  {
    motion = Left;
  }
  else
  {
    motion = Stop;
  }
}
void Fire()
{
  if(digitalRead(3))
  {
    gun = FIRE;
  }
  else
  {
    gun = HOLD;
  }
}
void setup() {
  // put your setup code here, to run once:
  for(int i = 0 ; i < 100 ; i++)
{
   bullet_process[i].y = 32; 
   bullet_process[i].x = 0; 

   enemy_bullets[i].y = 5;
   enemy_bullets[i].x = 0;
}

  timer.every(1, moveRocket);
  timer2.every(10, shoot);
 // timer3.every(10, enemy_shoot);

  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(9,INPUT);

  attachInterrupt(0,chooseMovement1,CHANGE);
  attachInterrupt(1,chooseMovement2,CHANGE);
  //attachInterrupt(1,Fire,CHANGE);
  display.begin();
  display.clearDisplay();
  for(char i = 0 ; i < sizeof(enemy_ships) ; i++)
  {
      display.drawBitmap(enemy_ships[i], 0,  enemy, 20, 10, BLACK);

  }
  display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, BLACK);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(motion != Stop)
  {      
      timer.tick(); // tick the timer
  }
  if(gun != HOLD)
  {      
      timer2.tick(); // tick the timer
  }
  //timer3.tick();
  sysUpdate();
}
void moveRocket(void *) 
{
  if((motion != Stop) && (motionPtr >= 0) && (motionPtr <= 58))
  {
    if(motion == Right)
    {
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, WHITE);
    motionPtr+=5;
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, BLACK);
    display.display(); 
    }
   else if(motion == Left)
    {
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, WHITE);
    motionPtr-=5;
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, BLACK);
    display.display(); 
    }    
  }
  else if(motionPtr < 5)
  {
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, WHITE);
    motionPtr = 5;
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, BLACK);
    display.display();    
  }
  else if(motionPtr > 58)
  {
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, WHITE);
    motionPtr = 58;
    display.drawBitmap(motionPtr, 32,  Rocket, 20, 10, BLACK);
    display.display();    
  }
}
void sysUpdate()
{
  static int iteration = 0;
  for(char i = 0 ; i < sizeof(enemy_ships) ; i++)
  {
      if(enemy_ships[i] == 0)
      {
        continue;
      }
      display.drawBitmap(abs(enemy_ships[i]), 0,  enemy, 20, 10, WHITE);
      if((abs(enemy_ships[i]) >= 63) || (abs(enemy_ships[i]) <= 1))
      {
          enemy_ships[i] *= -1;    
      }

         enemy_ships[i]++;
      if(!((abs(enemy_ships[i]) % 10)))
      {
              enemy_rear++;
              enemy_bullets[enemy_rear].x = abs(enemy_ships[i])+10; 
              enemy_bullets[enemy_rear].y = 5;
      }

      display.drawBitmap(abs(enemy_ships[i]), 0,  enemy, 20, 10, BLACK);
      display.display();    


  }
    if(digitalRead(9))
    {
      shoot();
    }
    for(int i = front+1 ; i <= rear ; i++){
    display.drawBitmap(bullet_process[i].x, bullet_process[i].y,  bullet, 2, 2, WHITE);
    
    if(bullet_process[i].y > 0)
    {
    bullet_process[i].y--;
    display.drawBitmap(bullet_process[i].x, bullet_process[i].y,  bullet, 2, 2, BLACK);
    }
    else
    {
        display.drawBitmap(bullet_process[front].x, bullet_process[front].y,  bullet, 2, 2, WHITE);
        front++;

    }
   for(char j = 0 ; j < sizeof(enemy_ships) ; j++)
   {
        if((bullet_process[i].x <= abs(enemy_ships[j]) + 10) && (bullet_process[i].x >= abs(enemy_ships[j])) && (bullet_process[i].y <= 5))
        {
            display.drawBitmap(abs(enemy_ships[j]), 0,  enemy, 20, 10, WHITE);
            enemy_ships[j] = 0;

        }
   }

         display.display(); 
         enemy_shoot();

//    if(iteration+front > rear)
//    {
//       iteration = front;
//    }
//    else
//    {
//      iteration++;
//      delay(10);
//      sysUpdate();
//    }
    delay(1);
    }
    
}
void shoot()
{
   
      rear++;
      bullet_process[rear].x = motionPtr+10; 
      bullet_process[rear].y = 32; 

    
}

void enemy_shoot()
{
    for(int i = enemy_front+1 ; i <= enemy_rear ; i++){
    display.drawBitmap(enemy_bullets[i].x, enemy_bullets[i].y,  bullet, 2, 2, WHITE);
    
    if(enemy_bullets[i].y < 40)
    {
    enemy_bullets[i].y++;
    display.drawBitmap(enemy_bullets[i].x, enemy_bullets[i].y,  bullet, 2, 2, BLACK);
    }
    else
    {
        display.drawBitmap(enemy_bullets[front].x, enemy_bullets[front].y,  bullet, 2, 2, WHITE);
        enemy_front++;

    }
//   for(char j = 0 ; j < sizeof(enemy_ships) ; j++)
//   {
//        if((bullet_process[i].x <= abs(enemy_ships[j]) + 10) && (bullet_process[i].x >= abs(enemy_ships[j])) && (bullet_process[i].y <= 5))
//        {
//            display.drawBitmap(abs(enemy_ships[j]), 0,  enemy, 20, 10, WHITE);
//            enemy_ships[j] = 0;
//
//        }
//   }

         display.display(); 
    }
}
