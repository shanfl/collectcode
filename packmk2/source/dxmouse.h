#include <dinput.h>
#include <d3dx9.h>

extern unsigned char keyBuffer[256];

#define MOUSE_DOWN(button) (mouseState.rgbButtons[button] & 0x80)
/* Mouse constants */
#define MOUSE_LEFT   0
#define MOUSE_RIGHT  1
#define MOUSE_MIDDLE 2

class dxmouse{
	private:
		LPDIRECTINPUT8		lpdi;			// Main DirectInput Object
		LPDIRECTINPUTDEVICE8	lpdiKeyboard;		// Keyboard's DirectInput Device
		LPDIRECTINPUTDEVICE8	lpdiMouse;		// Mouse's DirectInput Device (New)
		HWND		hWnd;
		HINSTANCE	hInstance;

	void GetWheelMovement(void);				// (New)
	void GetMouseMovement(void);				// (New)

	public:
		int x;						// Mouse Positions (New)
		int y;						// (New)
		int z;						// For Wheel Movement (New)

	bool Init(void);					// (Edited)
	void Shutdown(void);					// (Edited)
	void Update(void);					// (Edited)
	bool mousedown(int button);
	dxmouse(HWND hWnd, HINSTANCE hInstance)
		{	
			this->hWnd = hWnd;
			this->hInstance = hInstance;
			memset(&keyBuffer, 0, sizeof(unsigned char[256]));	
		}
	~dxmouse()
		{	}
};