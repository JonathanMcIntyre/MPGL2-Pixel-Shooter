/**********************************************************************************************************************
File: shooter-game.c                                                                

----------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------------------

Description:
This is a shooter-game.c file template 
This application is called "Pixel Shooter". It is a game where the user controls the gun on the left side of the screen
using the verticle captouch sliders and fire the gun at the incoming pixel blocks using button1.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void ShooterGameInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void ShooterGameRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32ShooterGameFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "ShooterGame_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type ShooterGame_StateMachine;            /* The state machine function pointer */
static u32 ShooterGame_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: ShooterGameInitialize

Description:
Initializes the State Machine and its variables.
Displays the opening message
*/
void ShooterGameInitialize(void)
{
  
  /* If good initialization, set state to Idle */
  if( 1 /* Add condition for good init */)
  {
    ShooterGame_StateMachine = Menu;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    ShooterGame_StateMachine = ShooterGameSM_FailedInit;
  }
  
  /* Display opening message */
  LcdClearScreen();
  u8 au8DataContent[] = "Push Button1 to Play";
  PixelAddressType loc1 = {15, 0};
  LcdLoadString(au8DataContent, LCD_FONT_SMALL, &loc1);
  
  LedOff(RED0);
  LedOff(RED1);
  LedOff(RED2);
  LedOff(RED3);
  LedOff(BLUE0);
  LedOff(BLUE1);
  LedOff(BLUE2);
  LedOff(BLUE3);
  LedOff(GREEN0);
  LedOff(GREEN1);
  LedOff(GREEN2);
  LedOff(GREEN3);
  
  
} /* end ShooterGameInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function ShooterGameRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer
*/
void ShooterGameRunActiveState(void)
{
  ShooterGame_StateMachine();
} /* end ShooterGameRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: PlaceObjectR
Description: Places a 3x1 pixel block at a random verticle position at the object's PixelColumnAddress
*/
void PlaceObjectR (PixelAddressType* object)
{
  object->u16PixelRowAddress = rand() % 63 + 1;
  LcdSetPixel(object); 
  object->u16PixelRowAddress++;
  LcdSetPixel(object); 
  object->u16PixelRowAddress -= 2;
  LcdSetPixel(object); 
  object->u16PixelRowAddress++;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: PlaceObjectR
Description: Places a 3x1 pixel block at the object's position
*/
void PlaceObject (PixelAddressType* object)
{
  LcdSetPixel(object); 
  object->u16PixelRowAddress++;
  LcdSetPixel(object); 
  object->u16PixelRowAddress -= 2;
  LcdSetPixel(object); 
  object->u16PixelRowAddress++;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: Fire
Description: Fires the gun by moving and removing the pixel bullet appropriately. This function is to be called in 
each iteration when a bullet is in motion.
*/
u16 Fire(u32 c, u16 y, PixelAddressType* object, PixelAddressType* object2, PixelAddressType* object3)
{
  PixelAddressType bullet = {y,8};
  bullet.u16PixelColumnAddress += (1200 - c) / 10;
  
  LcdClearPixel(&bullet);             // remove bullet from present location
  bullet.u16PixelColumnAddress++;     // update the bullet's location to one pixel to the right
  
  /**
  There are five cases:
  1. The bullet hits object1: returns 1
  2. The bullet hits object2: returns 1
  3. The bullet hits object3: returns 1
  4. The bullet misses (reaches the right end of the screen): returns 2
  5. The bullet is travelling: returns 0
  */
  
  /* target1 hit */
  if (   bullet.u16PixelColumnAddress >= object->u16PixelColumnAddress - 1          
      && bullet.u16PixelColumnAddress <= object->u16PixelColumnAddress + 1
        && bullet.u16PixelRowAddress >= object->u16PixelRowAddress - 1 
          && bullet.u16PixelRowAddress <= object->u16PixelRowAddress + 1)
  {
    LcdClearPixel(object);                 // remove target
    object->u16PixelRowAddress++;
    LcdClearPixel(object);
    object->u16PixelRowAddress -= 2;
    LcdClearPixel(object);
    object->u16PixelRowAddress++;
    object->u16PixelColumnAddress = 127;
    PlaceObjectR(object);                  // add new target
    return 1;
  }
  /* target2 hit */
  else if (bullet.u16PixelColumnAddress >= object2->u16PixelColumnAddress - 1        
           && bullet.u16PixelColumnAddress <= object2->u16PixelColumnAddress + 1
             && bullet.u16PixelRowAddress >= object2->u16PixelRowAddress - 1 
               && bullet.u16PixelRowAddress <= object2->u16PixelRowAddress + 1)
  {
    LcdClearPixel(object2);                 // remove target
    object2->u16PixelRowAddress++;
    LcdClearPixel(object2);
    object2->u16PixelRowAddress -= 2;
    LcdClearPixel(object2);
    object2->u16PixelRowAddress++;
    object2->u16PixelColumnAddress = 125;
    PlaceObjectR(object2);                  // add new target
    return 1;
  }
  /* target3 hit */
  else if (bullet.u16PixelColumnAddress >= object3->u16PixelColumnAddress - 1        
           && bullet.u16PixelColumnAddress <= object3->u16PixelColumnAddress + 1
             && bullet.u16PixelRowAddress >= object3->u16PixelRowAddress - 1 
               && bullet.u16PixelRowAddress <= object3->u16PixelRowAddress + 1)
  {
    LcdClearPixel(object3);                 // remove target
    object3->u16PixelRowAddress++;
    LcdClearPixel(object3);
    object3->u16PixelRowAddress -= 2;
    LcdClearPixel(object3);
    object3->u16PixelRowAddress++;
    object3->u16PixelColumnAddress = 123;
    PlaceObjectR(object3);                  // add new target
    return 1;
  }
  /* target miss */
  else if (bullet.u16PixelColumnAddress == 128)                                     
  {     
    return 2;
  }
  /* bullet is travelling */
  else                            
  {
    LcdSetPixel(&bullet);         // place the bullet at the location one pixel to the right
  }
  return 0;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: CenterGun
Description: Places the gun at the left side of the screen at the verticle position indicated by the offset argument
*/
void CenterGun(u8 offset)
{
  PixelAddressType gun = {offset + 1, 2};
  
  /* Clears any surrounding pixels from the gun's previous location */
  PixelAddressType Clearme = gun;
  Clearme.u16PixelRowAddress++;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelRowAddress -= 4;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelRowAddress++;
  Clearme.u16PixelColumnAddress++;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress++;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress++;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress++;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelRowAddress += 2;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress--;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress--;
  LcdClearPixel(&Clearme);
  Clearme.u16PixelColumnAddress--;
  LcdClearPixel(&Clearme);
  
  /* Places the gun */
  LcdSetPixel(&gun);
  gun.u16PixelRowAddress = offset-1;
  LcdSetPixel(&gun);
  gun.u16PixelRowAddress = offset;
  LcdSetPixel(&gun);
  gun.u16PixelColumnAddress = 3;
  LcdSetPixel(&gun);
  gun.u16PixelColumnAddress = 4;
  LcdSetPixel(&gun);
  gun.u16PixelColumnAddress = 5;
  LcdSetPixel(&gun);
  gun.u16PixelColumnAddress = 6;
  LcdSetPixel(&gun);
}



/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

static void Menu(void)
{
  if(WasButtonPressed(BUTTON1))                   // Push button1 to activate game
  { 
    ButtonAcknowledge(BUTTON1);
    // At the start, clear screen and place gun
    LcdClearScreen();
    CenterGun(32);
    CapTouchOn();
    ShooterGame_StateMachine = ShooterGame;
  } 
} /* end Menu() */

static void ShooterGame(void)
{
  static u32 count = 0;  
  static int score = 0;
  static u32 reload = 0;
  static u16 ybullet = 32;
  static u16 y = 32;                                          // position of the vertical slider
  static PixelAddressType objt1 = {32, 127};                  // obstacle location
  static PixelAddressType objt2 = {42, 125};                  // obstacle location
  static PixelAddressType objt3 = {22, 123};                  // obstacle location
  static u16 speed = 500;
  
  
  if (count == 0)                                             // Set object to destroy at right end of screen
  {
    PlaceObjectR(&objt1);
    PlaceObjectR(&objt2);
  }
  
  if (CaptouchCurrentVSlidePosition() < 255/2 - 40 && count % 100 == 0 && y > 0) // Lower y-position if Captouch is down every 100ms
  {
    y--;
  }
  
  if (CaptouchCurrentVSlidePosition() > 255/2 + 40 && count % 100 == 0 && y < 62)  // Raise y-position if Captouch is up every 100ms
  {
    y++;
  }
  
  if (count % 100 == 0)
  {
    CenterGun(y);             // Change gun position every 100ms
  }
  
  if (count % 1000 == 0 && speed > 90)          // make objects move faster as time goes on
  {
    speed -= 10;
  }
  
  if (count % speed == 0)
  {
    // Move objects closer
    LcdClearPixel(&objt1);                // remove target
    objt1.u16PixelRowAddress++;
    LcdClearPixel(&objt1);
    objt1.u16PixelRowAddress -= 2;
    LcdClearPixel(&objt1);
    objt1.u16PixelRowAddress++;
    objt1.u16PixelColumnAddress--;
    PlaceObject(&objt1);                  // add new target
    
    LcdClearPixel(&objt2);                // remove target
    objt2.u16PixelRowAddress++;
    LcdClearPixel(&objt2);
    objt2.u16PixelRowAddress -= 2;
    LcdClearPixel(&objt2);
    objt2.u16PixelRowAddress++;
    objt2.u16PixelColumnAddress--;
    PlaceObject(&objt2);                  // add new target
    
    LcdClearPixel(&objt3);                // remove target
    objt3.u16PixelRowAddress++;
    LcdClearPixel(&objt3);
    objt3.u16PixelRowAddress -= 2;
    LcdClearPixel(&objt3);
    objt3.u16PixelRowAddress++;
    objt3.u16PixelColumnAddress--;
    PlaceObject(&objt3);                  // add new target
  }
  
  if (count > 600)
  {
    if (IsButtonPressed(BUTTON1) && reload == 0)     // Fires a bullet if button1 is pushed and gun is reloaded
    {
      reload = 1200;    // create a 1200ms reload time
      ybullet = y;
    }
    if (reload > 0)     // reload if needed, and make bullet go to the right
    {
      u16 i = 0;
      if (reload % 10 == 0)
      {
        i = Fire(reload, ybullet, &objt1, &objt2, &objt3);
      }
      if (i == 1) 
      {
        score += 3;     // score increases by 3 if a target is destroyed
        reload = 0;
      }
      else if (i == 2) 
      {
        score--;       // score decreases by 1 for each miss
        reload = 0;
      }
      if (reload != 0) reload--;
    }
  }
  
  count++;
  
  // end game after a minute or if obstacle reaches left side
  if (count > 60000 || objt1.u16PixelColumnAddress == 0 ||objt2.u16PixelColumnAddress == 0 ||objt3.u16PixelColumnAddress == 0 )
  {
    LcdClearScreen();
    
    /* Game was lost because the obstacle reached the left side of the screen */
    if (objt1.u16PixelColumnAddress == 0 ||objt2.u16PixelColumnAddress == 0 ||objt3.u16PixelColumnAddress == 0) 
    {
      u8 au8DataContent0[] = "Your Score: 000";
      u8 au8DataContentL[] = "You Lose";
      PixelAddressType loc0 = {5, 0};
      LcdLoadString(au8DataContent0, LCD_FONT_SMALL, &loc0);
      PixelAddressType locL = {25, 0};
      LcdLoadString(au8DataContentL, LCD_FONT_SMALL, &locL);
    }
    /* Game was won. The player lasted a minute and prevented any obstacles from reaching the left side */
    else
    {
      if (score < 0) score = 0; 
      u8 digit100 = score / 100; 
      u8 digit10  = (score / 10) - (10*digit100);
      u8 digit1   = score % 10;
      
      u8 au8DataContent[] = "Your Score:    ";
      au8DataContent[12] = digit100 + 48;
      au8DataContent[13] = digit10  + 48;
      au8DataContent[14] = digit1  + 48;
      PixelAddressType loc1 = {5, 0};
      LcdLoadString(au8DataContent, LCD_FONT_SMALL, &loc1);
      u8 au8DataContentW[] = "You Win!";
      PixelAddressType locW = {25, 0};
      LcdLoadString(au8DataContentW, LCD_FONT_SMALL, &locW);
    }
    
    u8 au8DataContent2[] = "Push Button0 to Exit";
    PixelAddressType loc2 = {55, 0};
    LcdLoadString(au8DataContent2, LCD_FONT_SMALL, &loc2);
    
    CapTouchOff();
    PWMAudioOff(BUZZER1);
    
    count = 0;
    score = 0; 
    reload = 0;
    ybullet = 32;
    y = 32;
    objt1.u16PixelColumnAddress = 127;
    objt2.u16PixelColumnAddress = 125;
    objt3.u16PixelColumnAddress = 123;
    speed = 500;
    
    ShooterGame_StateMachine = GameOver;
  }
  
  
  if(IsButtonPressed(BUTTON0))                  // reset game if Button0 is pressed
  {
    LcdClearScreen();
    u8 au8DataContent[] = "Push Button1 to Play";
    PixelAddressType loc1 = {15, 0};
    LcdLoadString(au8DataContent, LCD_FONT_SMALL, &loc1);
    CapTouchOff();
    PWMAudioOff(BUZZER1);
    
    count = 0;
    score = 0; 
    reload = 0;
    ybullet = 32;
    y = 32;
    objt1.u16PixelColumnAddress = 127;
    objt2.u16PixelColumnAddress = 125;
    objt3.u16PixelColumnAddress = 123;
    speed = 500;
    
    ShooterGame_StateMachine = Menu;
  }
  
}

static void GameOver(void)
{
  if(IsButtonPressed(BUTTON0))
  {
    ShooterGame_StateMachine = Menu;
  } 
} /* end GameOver() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void ShooterGameSM_Error(void)          
{
  ShooterGame_StateMachine = Menu;  
} /* end ShooterGameSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void ShooterGameSM_FailedInit(void)          
{
  
} /* end ShooterGameSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
