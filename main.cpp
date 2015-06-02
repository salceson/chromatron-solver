#include <string>
#include <iostream>
#include <vector>

using namespace std;

struct Device {
    string name;
    int direction;
    bool movable;
    int color;
};

struct BoardElement {
    Device *device;
    int color;
};

BoardElement **board;
vector<Device *> settableDevices;

int width, height, devicesLength;

const bool DEBUG = true;

int stringToColor(string color) {
    if (color.length() != 3) {
        return 0;
    }
    int r = color[0] == '0' ? 0 : 1;
    int g = color[1] == '0' ? 0 : 1;
    int b = color[2] == '0' ? 0 : 1;
    return r << 2 | g << 1 | b;
}

string colorToString(int color) {
    int r = (color >> 2) & 0x1;
    int g = (color >> 1) & 0x1;
    int b = color & 0x1;
    string r_c = r == 1 ? "1" : "0";
    string g_c = g == 1 ? "1" : "0";
    string b_c = b == 1 ? "1" : "0";
    return r_c + g_c + b_c;
}

void readBoardDescription() {
    //Read board parameters
    cin >> width >> height >> devicesLength;

    //Create board
    board = (BoardElement **) malloc((height + 1) * sizeof(BoardElement *));
    for (int yPos = 0; yPos <= height; ++yPos) {
        board[yPos] = (BoardElement *) malloc((width + 1) * sizeof(BoardElement));
        for (int xPos = 0; xPos <= width; ++xPos) {
            Device *device = new Device;
            device->name = "EM";
            device->color = 0;
            device->direction = 0;
            device->movable = true;
            board[yPos][xPos].device = device;
            board[yPos][xPos].color = 0;
        }
    }

    //Read devices
    string deviceCode, color;
    int direction, x, y;
    for (int i = 0; i < devicesLength; ++i) {
        cin >> deviceCode >> x >> y >> direction >> color;
        if (deviceCode == "ES" || deviceCode == "RU" || deviceCode == "BL" || deviceCode == "TG") {
            board[y][x].device->name = deviceCode;
            board[y][x].device->movable = false;
            board[y][x].device->direction = direction;
            int parsedColor = stringToColor(color);
            board[y][x].device->color = parsedColor;
            board[y][x].color = parsedColor;
        }
        if (deviceCode == "LU" || deviceCode == "LP" || deviceCode == "LK") {
            Device *device = new Device;
            device->name = deviceCode;
            device->color = stringToColor(color);
            device->direction = direction;
            device->movable = true;
            settableDevices.push_back(device);
        }
    }
}

void printBoard() {
    if (DEBUG) {
        for (int y = 1; y <= height; ++y) {
            for (int x = 1; x <= width; ++x) {
                cout << board[y][x].device->name << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    cout << width << " " << height << endl << devicesLength << endl;
    for (int y = 0; y <= height; ++y) {
        for (int x = 0; x <= width; ++x) {
            if (board[y][x].device->name != "EM") {
                Device *device = board[y][x].device;
                string color = colorToString(device->color);
                cout << device->name << " " << x << " " << y << " " << device->direction << " " << color << endl;
            }
        }
    }
}

int main() {
    readBoardDescription();
    printBoard();
    return 0;
}
