#include "dxmouse.h"

DIMOUSESTATE  mouseState;	//The mouse's 'buffer'
UCHAR keyBuffer[256];		//The keyboard's key buffer

bool dxmouse::
	mousedown(int button){
	return MOUSE_DOWN(button);
}

bool dxmouse::
	Init(void)
	{
	HRESULT hr;	//DirectX result variable

//	S3Dlog.Output("DirectInput initiation:");

	//Create the main DirectInput object
	hr= (DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
						   IID_IDirectInput8, (void**)&lpdi, NULL));
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not create main DInput object", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not create main DInput object");
		return false;
		}

	//Create the keyboard's device object
	hr= (lpdi->CreateDevice(GUID_SysKeyboard, &lpdiKeyboard, NULL));
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not create keyboard's object",  "ERROR",MB_OK);
//		S3Dlog.Output("	Could not create main keyboard's object");
		Shutdown();
		return false;
		}

	//Set the keyboard's data format 
	hr= (lpdiKeyboard->SetDataFormat(&c_dfDIKeyboard));
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not set keyboard's data format","ERROR",MB_OK);
//		S3Dlog.Output("	Could not set keyboard's data format");
		Shutdown();
		return false;
		}

	//Set the keyboard's cooperation level with your computer's
	//environment.
	hr= (lpdiKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND|DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not set keyboard's cooperation level", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not set keyboard's cooperation level");
		Shutdown();
		return false;
		}

	//And finally, acquire the keyboard for use.
	lpdiKeyboard->Acquire();

	//Its time to get mousey
	hr= lpdi->CreateDevice(GUID_SysMouse, &lpdiMouse, NULL);
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not set create the mouse device", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not set create the mouse device");
		Shutdown();
		return FALSE;
		}

	//Set the mouse's data format
	hr= lpdiMouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr)) 
		{
		MessageBox(hWnd, "Could not set the mouse's data format", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not set the mouse's data format");
		Shutdown();
		return FALSE;
		}

	//Set the mouse's cooperative level with that of windows
	hr= lpdiMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr)) 
		{
		MessageBox(hWnd, "Could not set the mouse's behavior", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not set the mouse's behavior");
		Shutdown();
		return FALSE;
		}

	//Acquire the mouse
	hr= lpdiMouse->Acquire();
	if(FAILED(hr))
		{
		MessageBox(hWnd, "Could not acquire the mouse", "ERROR", MB_OK);
//		S3Dlog.Output("	Could not acquire the mouse");
		Shutdown();
		}

	//Set the mouse's device state
	hr= lpdiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	if(FAILED(hr))
		{
		MessageBox(hWnd, "The mouse has been lost in initialization", "ERROR", MB_OK);
//		S3Dlog.Output("	The mouse has been lost in initialization");
		Shutdown();
		}

	//Time to rock and roll
//	S3Dlog.Output("	DirectInput initialized successfully");
	return true;
	}
	
	
//------------------------------------------------------------------//
//- void dxmouse::Shutdown(void) ------------------------------//
//------------------------------------------------------------------//
//- This function shuts down DirectInput.  This  function is	   -//
//- automatically called if there is an error in the initiation	   -//
//- sequence, but if there was no error, you will need to call it  -//
//- from your shutdown function.								   -//
//------------------------------------------------------------------//	
void dxmouse::
	Shutdown(void)
	{
//	S3Dlog.Output("DirectInput shutdown:");

	//First, since we create the mouse last, we need to 'destroy'
	//it first.
	if(lpdiMouse!=NULL)
		{
		//Unacquire the mouse
		lpdiMouse->Unacquire();

		//Throw the mouse away
		lpdiMouse->Release();
		lpdiMouse= NULL;
		}

	if(lpdiKeyboard!=NULL)
		{
		//Unacquire the keyboard
		lpdiKeyboard->Unacquire();
		
		//Let the keyboard go faster than your ex-girlfriend's phone number
		lpdiKeyboard->Release();
		lpdiKeyboard= NULL;
		}
	
	//Now the main DirectInput object
	if(lpdi!=NULL)
		{
		//Kill, burn, and destroy the main object
		lpdi->Release();
		lpdi=NULL;
		}

//	S3Dlog.Output("	Everything has been shut down");
	}


//------------------------------------------------------------------//
//- void dxmouse::Update(void) --------------------------------//
//------------------------------------------------------------------//
//- This function updates DirectInput to make sure that your	   -//
//- program still has access to the keyboard and if it doesn't, the-//
//- function tries to reacquire it.  If not, the function calls	   -//
//- Shutdown().													   -//
//------------------------------------------------------------------//
void dxmouse::
	Update(void)
	{
	HRESULT hr;
	
	//First, check to see if the keyboard is still working/functioning
    hr= (lpdiKeyboard->GetDeviceState(sizeof(UCHAR[256]),(LPVOID)&keyBuffer));
    if(FAILED(hr))
		{ 
        if(hr==DIERR_INPUTLOST) 
			{
			//Try to re-acquire the keyboard
			hr= (lpdiKeyboard->Acquire());
			if(FAILED(hr))
				{
				MessageBox(hWnd, "Keyboard has been lost", "ERROR", MB_OK);
//				S3Dlog.Output("Keyboard has been lost");
				Shutdown();
				}
			}
		}

	//Now check the mouse
    hr= (lpdiMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&mouseState));
    if(FAILED(hr))
		{ 
        if(hr==DIERR_INPUTLOST) 
			{
			//Try to re-acquire the keyboard
			hr= (lpdiMouse->Acquire());
			if(FAILED(hr))
				{
				MessageBox(hWnd, "The mouse has been lost", "ERROR", MB_OK);
//				S3Dlog.Output("The mouse has been lost");
				Shutdown();
				}
			}
		}

	GetWheelMovement();
	GetMouseMovement();
	}

void dxmouse::
	GetWheelMovement(void)
	{	z= mouseState.lZ;	}

void dxmouse::
	GetMouseMovement(void)
	{
	x= mouseState.lX;
	y= mouseState.lY;
	}