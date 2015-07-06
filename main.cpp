#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

struct Device {
    string name;
    int direction;
    int color;
    int x;
    int y;
};

struct BoardElement {
    Device *device;
    int color;
    int colorDirections[8];
    bool checkedLP;
    bool checkedLU;
    bool checkedLK;
};

inline void propagateLight(int x, int y, int direction, int color);

inline int directionPlus180(int direction);

inline int directionToX(int direction);

inline int directionToY(int direction);

const char *s(const string &str) {
    return str.c_str();
}

BoardElement **board;
vector<Device *> settableDevices;
vector<Device *> lasers;
vector<Device *> targets;
vector<Device *> LUdevices;
vector<Device *> LPdevices;
vector<Device *> LKdevices;

int width, height, devicesLength;

const bool DEBUG = false;

inline int stringToColor(string color) {
    if (color.length() != 3) {
        return 0;
    }
    int r = color[0] == '0' ? 0 : 1;
    int g = color[1] == '0' ? 0 : 1;
    int b = color[2] == '0' ? 0 : 1;
    return r << 2 | g << 1 | b;
}

inline string colorToString(int color) {
    int r = (color >> 2) & 0x1;
    int g = (color >> 1) & 0x1;
    int b = color & 0x1;
    string r_c = r == 1 ? "1" : "0";
    string g_c = g == 1 ? "1" : "0";
    string b_c = b == 1 ? "1" : "0";
    string c = r_c + g_c + b_c;
    return c == "000" ? "0" : c;
}

void readBoardDescription() {
    //Read board parameters
    scanf("%d %d", &width, &height);
    scanf("%d", &devicesLength);

    //Create board
    board = (BoardElement **) malloc((height + 1) * sizeof(BoardElement *));
    for (int yPos = 0; yPos <= height; ++yPos) {
        board[yPos] = (BoardElement *) malloc((width + 1) * sizeof(BoardElement));
        for (int xPos = 0; xPos <= width; ++xPos) {
            Device *device = new Device;
            device->name = "EM";
            device->color = 0;
            device->direction = 0;
            board[yPos][xPos].device = device;
            board[yPos][xPos].color = 0x0;
            board[yPos][xPos].checkedLU = false;
            board[yPos][xPos].checkedLP = false;
            board[yPos][xPos].checkedLK = false;
            for (int dir = 0; dir < 8; ++dir) {
                board[yPos][xPos].colorDirections[dir] = 0x0;
            }
        }
    }

    //Read devices
    string deviceCode, color;
    int direction, x, y;
    for (int i = 0; i < devicesLength; ++i) {
        char *devCode = new char[3];
        char *col = new char[4];
        scanf("%2s %d %d %d %3s", devCode, &x, &y, &direction, col);
        deviceCode = string(devCode);
        color = string(col);
        board[y][x].device->x = x;
        board[y][x].device->y = y;
        if (deviceCode == "ES" || deviceCode == "RU" || deviceCode == "BL" || deviceCode == "TG") {
            board[y][x].device->name = deviceCode;
            board[y][x].device->direction = direction;
            int parsedColor = stringToColor(color);
            board[y][x].device->color = parsedColor;
            board[y][x].color = parsedColor;

            if (deviceCode == "ES") {
                lasers.push_back(board[y][x].device);
            }

            if (deviceCode == "TG") {
                targets.push_back(board[y][x].device);
            }
        }
        if (deviceCode == "LU" || deviceCode == "LP" || deviceCode == "LK") {
            Device *device = new Device;
            device->name = deviceCode;
            device->color = stringToColor(color);
            device->direction = direction;
            settableDevices.push_back(device);
            if (deviceCode == "LU") {
                LUdevices.push_back(device);
            }
            if (deviceCode == "LP") {
                LPdevices.push_back(device);
            }
            if (deviceCode == "LK") {
                LKdevices.push_back(device);
            }
        }
    }
}

void printBoard(bool colors) {
    if (DEBUG && !colors) {
        for (int y = 1; y <= height; ++y) {
            for (int x = 1; x <= width; ++x) {
                printf("%s ", s(board[y][x].device->name));
            }
            printf("\n");
        }
        printf("\n");
    }

    if (!colors) {
        printf("%d %d\n%d\n", width, height, devicesLength);
        for (int y = 0; y <= height; ++y) {
            for (int x = 0; x <= width; ++x) {
                if (board[y][x].device->name != "EM") {
                    Device *device = board[y][x].device;
                    string color = colorToString(device->color);
                    printf("%s %d %d %d %s\n", device->name.c_str(), x, y,
                           device->direction, color.c_str());
                }
            }
        }

        for (auto device : settableDevices) {
            if (device->x != 0 || device->y != 0) {
                continue;
            }
            printf("%s %d %d %d %s\n", device->name.c_str(), 0, 0, device->direction,
                   colorToString(device->color).c_str());
        }
    } else {
        for (int y = 1; y <= height; ++y) {
            for (int x = 1; x <= width; ++x) {
                printf("%s\t", colorToString(board[y][x].color).c_str());
            }
            printf("\n");
        }
    }
}

/*
 * Directions in the blocks:
 *  7  6  5
 *  0  *  4
 *  1  2  3
 */

const int luPropagation[8][8] = {
//        0   1   2   3   4   5   6   7
        {0,  7,  -1, -1, -1, -1, -1, 1}, //0
        {2,  1,  0,  -1, -1, -1, -1, -1}, //1
        {-1, 3,  2,  1,  -1, -1, -1, -1}, //2
        {-1, -1, 4,  3,  2,  -1, -1, -1}, //3
        {-1, -1, -1, 5,  4,  3,  -1, -1}, //4
        {-1, -1, -1, -1, 6,  5,  4,  -1}, //5
        {-1, -1, -1, -1, -1, 7,  6,  5}, //6
        {6,  1,  0,  -1, -1, -1, 0,  7}  //7
};

const int lkPropagation[8][8] = {
//        0   1   2   3   4   5   6   7
        {7,  6,  -1, -1, -1, -1, 1,  0}, //0
        {1,  0,  7,  -1, -1, -1, -1, 2}, //1
        {3,  2,  1,  0,  -1, -1, -1, -1}, //2
        {-1, 4,  3,  2,  1,  -1, -1, -1}, //3
        {-1, -1, 5,  4,  3,  2,  -1, -1}, //4
        {-1, -1, -1, 6,  5,  4,  3,  -1}, //5
        {-1, -1, -1, -1, 7,  6,  5,  4}, //6
        {5,  -1, -1, -1, -1, 0,  7,  6}  //7
};

const int lpPropagation[4][8] = {
//        0   1   2   3   4   5   6   7
        {-1, 7,  -1, 5,  -1, 3,  -1, 1}, //0
        {2,  -1, 0,  -1, 6,  -1, 4,  -1}, //1
        {-1, 3,  -1, 1,  -1, 7,  -1, 5}, //2
        {6,  -1, 4,  -1, 2,  -1, 0,  -1}, //3
};

inline int propagationDirection(Device *device, int x, int y, int direction, int color) {
    string name = device->name;
    int oppositeDirection = directionPlus180(direction);
    int dir = -1;
    if (name == "EM" || name == "TG") {
        dir = oppositeDirection;
    }
    if (name == "BL") {
        return -1;
    }
    if (name == "LU") {
        dir = luPropagation[device->direction][direction];
    }
    if (name == "LP") {
        int dx = directionToX(oppositeDirection), dy = directionToY(oppositeDirection);
        int newX = x + dx, newY = y + dy;
        if (newX >= 1 && newY >= 1 && newX <= width && newY <= height && (direction % 2) != (device->direction % 2)) {
            propagateLight(newX, newY, oppositeDirection, color);
        }
        dir = lpPropagation[device->direction][direction];
    }
    if (name == "LK") {
        dir = lkPropagation[device->direction][direction];
    }
    if (name == "ES") {
        dir = oppositeDirection;
    }
    if (name == "RU") {
        if (direction != device->direction && direction != directionPlus180(device->direction)) {
            return -1;
        }
        dir = oppositeDirection;
    }

    int dx = directionToX(dir);
    int dy = directionToY(dir);
    int newX = x + dx;
    int newY = y + dy;
    if (DEBUG) {
        printf("dir: %d newX: %d newY: %d\n", dir, newX, newY);
    }
    return newX >= 1 && newY >= 1 && newX <= width && newY <= height ? dir : -1;
}

inline int directionPlus180(int direction) {
    return (direction + 4) % 8;
}

inline int directionToX(int direction) {
    switch (direction) {
        case 0:
        case 1:
        case 7:
            return -1;
        case 2:
        case 6:
            return 0;
        default:
            return 1;
    }
}

inline int directionToY(int direction) {
    switch (direction) {
        case 1:
        case 2:
        case 3:
            return 1;
        case 0:
        case 4:
            return 0;
        default:
            return -1;
    }
}

inline void propagateLight(int x, int y, int direction, int color) {
    direction = directionPlus180(direction);
    //If we haven't checked the lights...
    while ((board[y][x].colorDirections[direction] & color) == 0x0) {
        board[y][x].color |= color;
        board[y][x].colorDirections[direction] |= color;
        int newDirection = propagationDirection(board[y][x].device, x, y, direction, color);
        if (newDirection == -1) {
            return;
        }
        int dx = directionToX(newDirection), dy = directionToY(newDirection);
        x += dx;
        y += dy;
        direction = directionPlus180(newDirection);
    }
}

void computeLights() {
    for (int y = 1; y <= height; ++y) {
        for (int x = 1; x <= width; ++x) {
            board[y][x].color = 0x0;
            for (int dir = 0; dir < 8; ++dir) {
                board[y][x].colorDirections[dir] = 0x0;
            }
        }
    }
    for (auto laser :lasers) {
        propagateLight(laser->x, laser->y, laser->direction, laser->color);
    }

    if (DEBUG) {
        printBoard(true);
    }
}

inline int previousDirection(int direction, int max) {
    int res = (direction - 1) % max;
    return res > 0 ? res : res + max;
}

inline int nextDirection(int direction, int max) {
    int res = (direction + 1) % max;
    return res > 0 ? res : res + max;
}

inline void getGoodDirectionsForDevice(string name, BoardElement *boardElement, bool *goodDirections) {
    for (int j = 0; j < 8; ++j) {
        goodDirections[j] = false;
    }

    if (name == "LU") {
        for (int i = 0; i < 8; ++i) {
            if (boardElement->colorDirections[i] != 0x0) {
                goodDirections[i] = true;
                goodDirections[previousDirection(i, 8)] = true;
                goodDirections[nextDirection(i, 8)] = true;
            }
        }
    }
    if (name == "LP") {
        for (int i = 0; i < 8; ++i) {
            if (boardElement->colorDirections[i] != 0x0) {
                goodDirections[i % 4] = true;
                goodDirections[previousDirection(i, 4)] = true;
                goodDirections[nextDirection(i, 4)] = true;
            }
        }
    }
    if (name == "LK") {
        for (int i = 0; i < 8; ++i) {
            if (boardElement->colorDirections[i] != 0x0) {
                goodDirections[i] = true;
                goodDirections[previousDirection(i, 8)] = true;
                goodDirections[nextDirection(i, 8)] = true;
                goodDirections[nextDirection(nextDirection(i, 8), 8)] = true;
            }
        }
    }
}

bool checkIfDone() {
    for (auto target :targets) {
        if (board[target->y][target->x].color != target->color) {
            return false;
        }
    }
    return true;
}

bool solve(bool mark) {
    computeLights();

    int lights[51][51];

    int lightsDir[51][51][8];


    for (int i = 1; i <= width; ++i) {
        for (int j = 1; j <= height; ++j) {
            lights[j][i] = board[j][i].color;
            for (int k = 0; k < 8; ++k) {
                lightsDir[j][i][k] = board[j][i].colorDirections[k];
            }
        }
    }

    if (checkIfDone()) {
        return true;
    }

    bool goodDirections[8] = {false};

    for (int y = 1; y <= height; ++y) {
        for (int x = 1; x <= width; ++x) {
            if (board[y][x].device->name != "EM" || board[y][x].color == 0x0) {
                continue;
            }

            Device *dev = board[y][x].device;

            if (!board[y][x].checkedLU && LUdevices.size() > 0) {
                Device *lu = LUdevices[LUdevices.size() - 1];
                LUdevices.pop_back();
                lu->x = x;
                lu->y = y;
                for (int i = 1; i <= width; ++i) {
                    for (int j = 1; j <= height; ++j) {
                        board[j][i].color = lights[j][i];
                        for (int k = 0; k < 8; ++k) {
                            board[j][i].colorDirections[k] = lightsDir[j][i][k];
                        }
                    }
                }
                getGoodDirectionsForDevice("LU", &board[y][x], goodDirections);

                for (int dir = 0; dir < 8; ++dir) {
                    if (!goodDirections[dir]) {
                        continue;
                    }

                    if (DEBUG) {
                        printf("LU %d %d %d\n", x, y, dir);
                    }
                    board[y][x].device = lu;
                    lu->direction = dir;
                    if (solve(false)) {
                        return true;
                    }
                }

                lu->x = 0;
                lu->y = 0;
                LUdevices.push_back(lu);
                board[y][x].device = dev;
                if (mark) {
                    board[y][x].checkedLU = true;
                }
            }

            if (!board[y][x].checkedLP && LPdevices.size() > 0) {
                Device *lp = LPdevices[LPdevices.size() - 1];
                LPdevices.pop_back();
                lp->x = x;
                lp->y = y;
                for (int i = 1; i <= width; ++i) {
                    for (int j = 1; j <= height; ++j) {
                        board[j][i].color = lights[j][i];
                        for (int k = 0; k < 8; ++k) {
                            board[j][i].colorDirections[k] = lightsDir[j][i][k];
                        }
                    }
                }
                getGoodDirectionsForDevice("LP", &board[y][x], goodDirections);

                for (int dir = 0; dir < 4; ++dir) {
                    if (!goodDirections[dir]) {
                        continue;
                    }

                    if (DEBUG) {
                        printf("LP %d %d %d\n", x, y, dir);
                    }
                    board[y][x].device = lp;
                    lp->direction = dir;
                    if (solve(false)) {
                        return true;
                    }
                }

                lp->x = 0;
                lp->y = 0;
                LPdevices.push_back(lp);
                board[y][x].device = dev;
                if (mark) {
                    board[y][x].checkedLP = true;
                }
            }

            if (!board[y][x].checkedLK && LKdevices.size() > 0) {
                Device *lk = LKdevices[LKdevices.size() - 1];
                LKdevices.pop_back();
                lk->x = x;
                lk->y = y;
                for (int i = 1; i <= width; ++i) {
                    for (int j = 1; j <= height; ++j) {
                        board[j][i].color = lights[j][i];
                        for (int k = 0; k < 8; ++k) {
                            board[j][i].colorDirections[k] = lightsDir[j][i][k];
                        }
                    }
                }
                getGoodDirectionsForDevice("LK", &board[y][x], goodDirections);

                for (int dir = 0; dir < 8; ++dir) {
                    if (!goodDirections[dir]) {
                        continue;
                    }

                    if (DEBUG) {
                        printf("LK %d %d %d\n", x, y, dir);
                    }
                    board[y][x].device = lk;
                    lk->direction = dir;
                    if (solve(false)) {
                        return true;
                    }
                }

                lk->x = 0;
                lk->y = 0;
                LKdevices.push_back(lk);
                board[y][x].device = dev;
                if (mark) {
                    board[y][x].checkedLK = true;
                }
            }
            for (int i = 1; i <= width; ++i) {
                for (int j = 1; j <= height; ++j) {
                    board[j][i].color = lights[j][i];
                    for (int k = 0; k < 8; ++k) {
                        board[j][i].colorDirections[k] = lightsDir[j][i][k];
                    }
                }
            }
        }
    }

    return false;
}

int main() {
    readBoardDescription();
    if (!solve(true)) {
        return 1;
    }
    printBoard(false);
    return 0;
}
