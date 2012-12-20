/*-----------------------------------------------------------------------------

  Console.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  This module is not at all needed.  It's just an output window used for 
  debugging.  

  Sadly, I had some proprietary code in here, so I couldn't release it. 
  Still, this thing is pretty simple: Make a window, and when other modules
  call Console (), just print the text to the window.  The program works
  just fine without this, but development is easier when you can print stuff 
  to the console and get feedback while it's building.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>

/*-----------------------------------------------------------------------------
                                   
-----------------------------------------------------------------------------*/

void Console (char *message, ...)
{

  char              msg_text[256];
  va_list           marker;

  va_start (marker, message);
  vsprintf (msg_text, message, marker);
  va_end (marker);
  //Now the message is stored in msg_text, you should probably display
  //it or something...

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void ConsoleTerm (void)
{

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void ConsoleInit (void)
{

}