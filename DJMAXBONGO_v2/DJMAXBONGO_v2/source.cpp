#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>

enum keyModes { key4B = 0, key5B, key6B, key8B };
enum excModes { M_Viewer = 0, M_Change , M_KChange };

int keyNum[4]{ 6, 8, 8, 10 };
int keyMd[4]{ 4, 5, 6, 8 };
int keys4B[6], keys5B[8], keys6B[8], keys8B[10];
int* keyInfo[4]{ keys4B, keys5B, keys6B, keys8B };

bool k4Bpr[6], k5Bpr[8], k6Bpr[8], k8Bpr[10];
int curKeyMode = key4B;
int curExcMode = M_Viewer;
bool curKeyPress[10];

std::string keyNames[4][10]{
	{"KEY 1", "KEY 2", "KEY 3", "KEY 4", "LEFT SIDE", "RIGHT SIDE"},
	{"KEY 1", "KEY 2", "KEY 3", "KEY 4", "KEY 5", "KEY 6", "LEFT SIDE", "RIGHT SIDE"},
	{"KEY 1", "KEY 2", "KEY 3", "KEY 4", "KEY 5", "KEY 6", "LEFT SIDE", "RIGHT SIDE"},
	{"KEY 1", "KEY 2", "KEY 3", "KEY 4", "KEY 5", "KEY 6", "LEFT TRACK" , "RIGHT TRACK" , "LEFT SIDE", "RIGHT SIDE"}
};

sf::Texture texture[25];
sf::Sprite Cats, Paw1, Paw2, Paw3, Paw4;
sf::RenderWindow window(sf::VideoMode(1280, 720), "DJMAX BONGO CAT");
int textureNum[4]{ 12, 16, 16, 24 };

void keyInit();
void keySave();
void keyChange(keyModes);

int loadTexture(keyModes);
void drawScene(sf::RenderWindow&, int, int, int, int);

void gotoxy(int, int);

const int K_CHANGEKEY = VK_OEM_MINUS;
const int K_CHANGEMODE = VK_OEM_PLUS;

std::string clear = "";


int main() {
	for (int i = 0; i < 11; i++) clear += "                          \n";

	keyInit();
	std::string djct = "assets\\djmaxcats_" + std::to_string(keyMd[curKeyMode]);
	if (!texture[0].loadFromFile(djct + ".png")) return EXIT_FAILURE;
	for (int i = 1; i <= textureNum[curKeyMode]; i++) {
		if (!texture[i].loadFromFile(djct + "_" + std::to_string(i) + ".png"))
			return EXIT_FAILURE;
	}

	window.clear();
	std::cout << "Current Key Mode: " << keyMd[curKeyMode] << "B\n";

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close(); break;
			case (sf::Event::Resized || sf::Event::GainedFocus || sf::Event::LostFocus):
				break;
			}
		}
		window.clear();
		switch (curExcMode) {
		case M_Viewer:
			if ((GetAsyncKeyState(K_CHANGEKEY) & 0x8000)) curExcMode = M_Change;
			else if ((GetAsyncKeyState(K_CHANGEMODE) & 0x8000)) curExcMode = M_KChange;
			else {
				for (int i = 0; i < keyNum[curKeyMode]; i++) {
					curKeyPress[i] = (bool)(GetAsyncKeyState(keyInfo[curKeyMode][i]) & 0x8000);
				}
				int ipaw1, ipaw2, ipaw3, ipaw4;
				switch (curKeyMode) {
				case key4B:
					ipaw1 = (int)curKeyPress[4] + ((int)curKeyPress[0] << 1) + 1; // LLPaw / Paw1
					ipaw2 = (int)curKeyPress[1] + 5; // LRPaw / Paw2
					ipaw3 = (int)curKeyPress[2] + 7; // RLPaw / Paw3
					ipaw4 = (int)curKeyPress[3] + ((int)curKeyPress[5] << 1) + 9; // RRPaw / Paw4
					break;
				case key5B:
				case key6B:
					ipaw1 = (int)curKeyPress[6] + ((int)curKeyPress[0] << 1) + 1; // LLPaw / Paw1
					ipaw2 = (int)curKeyPress[1] + ((int)curKeyPress[2] << 1) + 5; // LRPaw / Paw2
					ipaw3 = (int)curKeyPress[3] + ((int)curKeyPress[4] << 1) + 9; // LRPaw / Paw3
					ipaw4 = (int)curKeyPress[5] + ((int)curKeyPress[7] << 1) + 13; // LRPaw / Paw4
					break;
				case key8B:
					ipaw1 = (int)curKeyPress[8] + ((int)curKeyPress[0] << 1) + ((int)curKeyPress[6] << 2) + 1; // LLPaw / Paw1
					ipaw2 = (int)curKeyPress[1] + ((int)curKeyPress[2] << 1) + 9; // LRPaw / Paw2
					ipaw3 = (int)curKeyPress[3] + ((int)curKeyPress[4] << 1) + 13; // LRPaw / Paw3
					ipaw4 = (int)curKeyPress[5] + ((int)curKeyPress[7] << 1) + ((int)curKeyPress[9] << 2) + 17; // LRPaw / Paw4
					break;
				}
				drawScene(window, ipaw1, ipaw2, ipaw3, ipaw4);
				sf::sleep(sf::milliseconds(15));
			}
			break;
		case M_Change:
			if (!(GetAsyncKeyState(K_CHANGEKEY) & 0x8000)) {
				std::cout << "Key Change Mode: " << keyMd[curKeyMode] << "B\n";
				keyChange(keyModes(curKeyMode));
				curExcMode = M_Viewer;
				for (bool& rs : curKeyPress) rs = false;
			}
			break;
		case M_KChange:
			if (!(GetAsyncKeyState(K_CHANGEMODE) & 0x8000)) {
				curExcMode = M_Viewer;
				curKeyMode = (curKeyMode + 1) % 4;
				gotoxy(0, 0);
				std::cout << "Current Key Mode: " << keyMd[curKeyMode] << "B\n";
				loadTexture(keyModes(curKeyMode));
				gotoxy(0, 1);
				std::cout << clear;
				gotoxy(0, 1);
			}
			break;
		}
	}
	return EXIT_SUCCESS;
}

// FUNCTIONS

void keyInit() {
	// load key code info
	std::ifstream ifs("Keys.txt");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < keyNum[i]; j++) {
			ifs >> keyInfo[i][j];
		}
	}
}

void keySave() {
	std::ofstream ofs("Keys.txt");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < keyNum[i]; j++) {
			ofs << keyInfo[i][j] << ' ';
		}
		ofs << '\n';
	}
}

void keyChange(keyModes keyMode) {
	const int debounceDelay = 50;
	int count = 0;
	bool keyState[256]{ false };
	bool stdcout = true;

	gotoxy(0, 1);
	std::cout << clear;
	gotoxy(0, 1);

	while (count < keyNum[keyMode]) {
		if (stdcout) {
			std::cout << "New " << keyNames[keyMode][count] << " Key: ";
			stdcout = false;
		}
		for (int key = 8; key < 256; key++) {
			if (key == VK_SHIFT) continue;
			if (key >= VK_F1 && key <= VK_F24) continue;
			bool keyPressState = GetAsyncKeyState(key) & 0x8000;
			if (keyPressState && !keyState[key]) {
				keyInfo[keyMode][count] = key;
				keyState[key] = true;
				stdcout = true;
				count++;
				std::cout << "0x" << std::hex << key << '\n';
			}
			else if (!keyPressState) {
				keyState[key] = false;
			}
		}
		Sleep(debounceDelay);
	}
	keySave();
}

int loadTexture(keyModes keyMode) {
	std::string djct = "assets\\djmaxcats_" + std::to_string(keyMd[keyMode]);
	if (!texture[0].loadFromFile(djct + ".png")) return EXIT_FAILURE;
	for (int i = 1; i <= textureNum[keyMode]; i++) {
		if (!texture[i].loadFromFile(djct + "_" + std::to_string(i) + ".png"))
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void drawScene(sf::RenderWindow& rw, int i1, int i2, int i3, int i4) {
	Paw1.setTexture(texture[i1]);
	Paw2.setTexture(texture[i2]);
	Paw3.setTexture(texture[i3]);
	Paw4.setTexture(texture[i4]);
	Cats.setTexture(texture[0]);
	rw.draw(Cats);
	rw.draw(Paw1);
	rw.draw(Paw2);
	rw.draw(Paw3);
	rw.draw(Paw4);
	rw.display();
}

void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}