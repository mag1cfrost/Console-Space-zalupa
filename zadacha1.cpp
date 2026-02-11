#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <conio.h>

using namespace std;

const double PI = 3.14159265358979323846;

double scale = 1.0;
int offsetX = 0, offsetY = 0;
const double MIN_SCALE = 0.3;
const double MAX_SCALE = 3.0;

struct Star {
    int x;
    int y;
    int color;
    char symbol;
    int brightness;
};

struct TrailPoint {
    int x;
    int y;
    int brightness;
    int color;
};

struct Planet {
    int x;
    int y;
    int color;
    char symbol;
    int orbitRadius;
    double angle;
    double speed;
    int size;
    vector<TrailPoint> trail;
    int maxTrailLength;
    string name;
};

struct Comet {
    int x;
    int y;
    int color;
    double speedX;
    double speedY;
    int life;
    vector<TrailPoint> trail;
    int maxTrailLength;
};

struct Starship {
    int x;
    int y;
    int color;
    char symbol;
    double speedX;
    double speedY;
    int direction;
    vector<TrailPoint> trail;
    int maxTrailLength;
    int warpCooldown;
};

void worldToScreen(int worldX, int worldY, int centerX, int centerY, int& screenX, int& screenY) {
    screenX = centerX + (int)((worldX - centerX) * scale) + offsetX;
    screenY = centerY + (int)((worldY - centerY) * scale) + offsetY;
}

void getConsoleSize(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    else {
        width = 80;
        height = 24;
    }
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void setCursorPosition(int x, int y) {
    if (x < 0 || y < 0) return;

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void handleInput() {
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
        case '+':
            scale = min(MAX_SCALE, scale * 1.1);
            break;
        case '-':
            scale = max(MIN_SCALE, scale / 1.1);
            break;
        case 'w': case 'W':
            offsetY -= 5;
            break;
        case 's': case 'S':
            offsetY += 5;
            break;
        case 'a': case 'A':
            offsetX -= 5;
            break;
        case 'd': case 'D':
            offsetX += 5;
            break;
        case 'r': case 'R':
            scale = 1.0;
            offsetX = 0;
            offsetY = 0;
            break;
        }
    }
}

vector<Star> createStarfield(int width, int height) {
    vector<Star> stars;

    int starColors[] = { 7, 8, 15, 14, 11, 9, 10, 12, 13 };
    int colorCount = 9;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (rand() % 100 < 6) {
                Star star;
                star.x = x;
                star.y = y;
                star.color = starColors[rand() % colorCount];
                star.symbol = (rand() % 3 == 0) ? '*' : (rand() % 2 == 0) ? '.' : '+';
                star.brightness = 50 + rand() % 50;
                stars.push_back(star);
            }
        }
    }

    return stars;
}

vector<Planet> createRandomPlanets(int centerX, int centerY, int width, int height) {
    vector<Planet> planets;

    string planetNames[] = { "Меркурий", "Венера", "Земля", "Марс", "Юпитер", "Сатурн", "Уран", "Нептун", "Кеплер", "Церера" };

    int planetCount = 5 + rand() % 4;

    for (int i = 0; i < planetCount; i++) {
        Planet planet;

        planet.size = 1 + rand() % 3;

        int maxOrbit = min(width, height) / 2 - 5;
        if (maxOrbit < 15) maxOrbit = 15;

        planet.orbitRadius = 8 + rand() % (max(1, maxOrbit - 8));

        planet.angle = (rand() % 360) * PI / 180.0;

        double baseSpeed = 0.003 + (rand() % 10) / 1500.0;
        planet.speed = baseSpeed * (30.0 / max(1, planet.orbitRadius));

        int colors[] = { 1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 14, 15 };
        planet.color = colors[rand() % 13];

        char symbols[] = { 'O', 'o', '0', 'Q', '@', '°', '¤', '●', '♦' };
        planet.symbol = symbols[rand() % 9];

        planet.maxTrailLength = 30 + rand() % 40;
        planet.trail.clear();
        planet.name = planetNames[rand() % 10];

        planets.push_back(planet);
    }

    return planets;
}

vector<Comet> createComets(int width, int height) {
    vector<Comet> comets;

    int cometCount = 1 + rand() % 3;

    for (int i = 0; i < cometCount; i++) {
        Comet comet;

        if (rand() % 2 == 0) {
            comet.x = 0;
            comet.speedX = 0.5 + (rand() % 10) / 10.0;
        }
        else {
            comet.x = width - 1;
            comet.speedX = -0.5 - (rand() % 10) / 10.0;
        }

        comet.y = rand() % max(1, height);
        comet.speedY = (rand() % 10 - 5) / 20.0;

        int colors[] = { 11, 14, 15, 10, 13 };
        comet.color = colors[rand() % 5];

        comet.life = 100 + rand() % 100;
        comet.maxTrailLength = 40;
        comet.trail.clear();

        comets.push_back(comet);
    }

    return comets;
}

vector<Starship> createStarships(int width, int height) {
    vector<Starship> starships;

    int shipCount = 2 + rand() % 4;

    for (int i = 0; i < shipCount; i++) {
        Starship ship;

        ship.x = rand() % max(1, width);
        ship.y = rand() % max(1, height);

        if (rand() % 2 == 0) {
            ship.speedX = 0.3 + (rand() % 10) / 15.0;
        }
        else {
            ship.speedX = -0.3 - (rand() % 10) / 15.0;
        }

        ship.speedY = (rand() % 10 - 5) / 25.0;

        int colors[] = { 9, 10, 11, 12, 13, 14, 15 };
        ship.color = colors[rand() % 7];

        char symbols[] = { '>', '<', '^', 'A', 'V', 'Φ', 'Ψ', '╦', '╣', '╠', '╩' };
        ship.symbol = symbols[rand() % 11];
        ship.direction = (ship.speedX > 0) ? 1 : 0;

        ship.maxTrailLength = 25;
        ship.trail.clear();
        ship.warpCooldown = 0;

        starships.push_back(ship);
    }

    return starships;
}

void updatePlanets(vector<Planet>& planets, int centerX, int centerY, int width, int height) {
    for (Planet& planet : planets) {
        TrailPoint point;
        point.x = planet.x;
        point.y = planet.y;
        point.brightness = 100;
        point.color = planet.color;
        planet.trail.push_back(point);

        for (int i = 0; i < planet.trail.size(); i++) {
            int distanceFromEnd = planet.trail.size() - i;
            planet.trail[i].brightness = max(0, 100 - (distanceFromEnd * 100 / planet.maxTrailLength));
        }

        if (planet.trail.size() > planet.maxTrailLength) {
            planet.trail.erase(planet.trail.begin());
        }

        planet.angle += planet.speed;
        if (planet.angle > 2 * PI) {
            planet.angle -= 2 * PI;
        }

        planet.x = centerX + (int)(planet.orbitRadius * sin(planet.angle));
        planet.y = centerY + (int)(planet.orbitRadius * cos(planet.angle) * 0.3);

        planet.x = max(0, min(width - 1, planet.x));
        planet.y = max(0, min(height - 1, planet.y));
    }
}

void updateComets(vector<Comet>& comets, int width, int height) {
    for (Comet& comet : comets) {
        TrailPoint point;
        point.x = comet.x;
        point.y = comet.y;
        point.brightness = 100;
        point.color = comet.color;
        comet.trail.push_back(point);

        for (int i = 0; i < comet.trail.size(); i++) {
            int distanceFromEnd = comet.trail.size() - i;
            comet.trail[i].brightness = max(0, 100 - (distanceFromEnd * 100 / comet.maxTrailLength));
        }

        if (comet.trail.size() > comet.maxTrailLength) {
            comet.trail.erase(comet.trail.begin());
        }

        comet.x += (int)comet.speedX;
        comet.y += (int)comet.speedY;

        comet.life--;

        if (comet.x < -10 || comet.x > width + 10 || comet.y < -10 || comet.y > height + 10 || comet.life <= 0) {
            if (rand() % 2 == 0) {
                comet.x = 0;
                comet.speedX = 0.5 + (rand() % 10) / 10.0;
            }
            else {
                comet.x = width - 1;
                comet.speedX = -0.5 - (rand() % 10) / 10.0;
            }

            comet.y = rand() % max(1, height);
            comet.speedY = (rand() % 10 - 5) / 20.0;
            comet.life = 100 + rand() % 100;
            comet.trail.clear();
        }
    }
}

void updateStarships(vector<Starship>& starships, int width, int height) {
    for (Starship& ship : starships) {
        TrailPoint point;
        point.x = ship.x;
        point.y = ship.y;
        point.brightness = 100;
        point.color = ship.color;
        ship.trail.push_back(point);

        for (int i = 0; i < ship.trail.size(); i++) {
            int distanceFromEnd = ship.trail.size() - i;
            ship.trail[i].brightness = max(0, 100 - (distanceFromEnd * 100 / ship.maxTrailLength));
        }

        if (ship.trail.size() > ship.maxTrailLength) {
            ship.trail.erase(ship.trail.begin());
        }

        if (ship.warpCooldown > 0) {
            ship.warpCooldown--;
        }

        if (ship.warpCooldown == 0 && rand() % 100 < 6) {
            for (int i = 0; i < 10; i++) {
                TrailPoint warpPoint;
                warpPoint.x = ship.x + (rand() % 21 - 10);
                warpPoint.y = ship.y + (rand() % 21 - 10);
                warpPoint.brightness = 70 - i * 7;
                warpPoint.color = 13;
                ship.trail.push_back(warpPoint);
            }

            ship.x = rand() % width;
            ship.y = rand() % height;

            ship.warpCooldown = 100 + rand() % 100;

            if (ship.trail.size() > 10) {
                ship.trail.erase(ship.trail.begin(), ship.trail.end() - 10);
            }
        }
        else {
            ship.x += (int)ship.speedX;
            ship.y += (int)ship.speedY;
        }

        if (ship.x < 0) {
            ship.x = width - 1;
        }
        else if (ship.x >= width) {
            ship.x = 0;
        }

        if (ship.y < 0) {
            ship.y = height - 1;
        }
        else if (ship.y >= height) {
            ship.y = 0;
        }

        if (rand() % 100 == 0) {
            ship.speedY = (rand() % 10 - 5) / 25.0;
        }
    }
}

void drawBlackHole(int centerX, int centerY, int width, int height, int frame) {
    if (centerX - 3 < 0 || centerX + 3 >= width || centerY - 3 < 0 || centerY + 3 >= height) {
        return;
    }

    int pulse = (frame % 60) / 10;

    setColor(0);

    char centerChar;
    switch (pulse) {
    case 0: centerChar = ' '; break;
    case 1: centerChar = '.'; break;
    case 2: centerChar = ':'; break;
    case 3: centerChar = 'o'; break;
    case 4: centerChar = 'O'; break;
    case 5: centerChar = '0'; break;
    default: centerChar = ' '; break;
    }

    setCursorPosition(centerX - 2, centerY - 1);
    cout << "█████";
    setCursorPosition(centerX - 2, centerY);
    cout << "██" << centerChar << "██";
    setCursorPosition(centerX - 2, centerY + 1);
    cout << "█████";

    setColor(8);
    double rotation = (frame % 360) * PI / 180.0;

    for (int i = 0; i < 12; i++) {
        double angle = rotation + (i * PI / 6);
        double wave = sin(frame * 0.1 + i * 0.5) * 0.5 + 1.0;
        int radius = (int)(3.0 * wave);

        int x = centerX + (int)(radius * cos(angle));
        int y = centerY + (int)(radius * sin(angle) * 0.7);

        if (x >= 0 && x < width && y >= 0 && y < height) {
            setCursorPosition(x, y);
            char diskChar;
            if (i % 3 == 0) {
                diskChar = '·';
                setColor(7);
            }
            else if (i % 3 == 1) {
                diskChar = '°';
                setColor(8);
            }
            else {
                diskChar = 'o';
                setColor(15);
            }
            cout << diskChar;
        }
    }

    setColor(7);
    for (int i = 0; i < 6; i++) {
        double angle = rotation * 2 + (i * PI / 3);
        int radius = 7;

        int x = centerX + (int)(radius * cos(angle));
        int y = centerY + (int)(radius * sin(angle) * 0.7);

        if (x >= 0 && x < width && y >= 0 && y < height) {
            setCursorPosition(x, y);
            cout << '~';
        }
    }
}

void drawTrail(const vector<TrailPoint>& trail, int width, int height) {
    for (const TrailPoint& point : trail) {
        if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height && point.brightness > 0) {
            setCursorPosition(point.x, point.y);

            int displayColor;
            char symbol;

            if (point.brightness > 80) {
                displayColor = point.color;
                symbol = '•';
            }
            else if (point.brightness > 60) {
                displayColor = point.color;
                symbol = '*';
            }
            else if (point.brightness > 40) {
                displayColor = 8;
                symbol = '.';
            }
            else if (point.brightness > 20) {
                displayColor = 8;
                symbol = '·';
            }
            else {
                displayColor = 0;
                symbol = ' ';
            }

            setColor(displayColor);
            cout << symbol;
        }
    }
}

void drawPlanets(const vector<Planet>& planets, int width, int height) {
    for (const Planet& planet : planets) {
        drawTrail(planet.trail, width, height);

        if (planet.x >= 0 && planet.x < width && planet.y >= 0 && planet.y < height) {
            setCursorPosition(planet.x, planet.y);
            setColor(planet.color);
            cout << planet.symbol;

            if (planet.size >= 2) {
                setCursorPosition(planet.x - 1, planet.y);
                cout << "-";
                setCursorPosition(planet.x + 1, planet.y);
                cout << "-";
            }
            if (planet.size >= 3) {
                setCursorPosition(planet.x, planet.y - 1);
                cout << "|";
                setCursorPosition(planet.x, planet.y + 1);
                cout << "|";
            }
        }
    }
}

void drawComets(const vector<Comet>& comets, int width, int height) {
    for (const Comet& comet : comets) {
        drawTrail(comet.trail, width, height);

        if (comet.x >= 0 && comet.x < width && comet.y >= 0 && comet.y < height) {
            setCursorPosition(comet.x, comet.y);
            setColor(comet.color);
            cout << '☄';
        }
    }
}

void drawStarships(const vector<Starship>& starships, int width, int height) {
    for (const Starship& ship : starships) {
        drawTrail(ship.trail, width, height);

        if (ship.x >= 0 && ship.x < width && ship.y >= 0 && ship.y < height) {
            setCursorPosition(ship.x, ship.y);
            setColor(ship.color);
            cout << ship.symbol;
        }
    }
}

void drawStars(const vector<Star>& stars, int width, int height, int frame) {
    for (const Star& star : stars) {
        if (star.x >= 0 && star.x < width && star.y >= 0 && star.y < height) {
            setCursorPosition(star.x, star.y);

            int flicker = (frame + star.x + star.y) % 100;
            int currentBrightness;

            if (flicker < 10) {
                currentBrightness = star.brightness + 30;
            }
            else if (flicker < 20) {
                currentBrightness = star.brightness - 20;
            }
            else {
                currentBrightness = star.brightness;
            }

            currentBrightness = max(20, min(100, currentBrightness));

            if (currentBrightness > 70) {
                setColor(star.color);
            }
            else if (currentBrightness > 40) {
                setColor(8);
            }
            else {
                setColor(0);
                cout << ' ';
                continue;
            }

            cout << star.symbol;
        }
    }
}

int main() {
    srand(time(0));
    hideCursor();
    system("title Улучшенная Космическая Вселенная с Масштабированием");

    cout << "Улучшенная космическая вселенная с масштабированием!" << endl;
    cout << "Управление:" << endl;
    cout << "  +/- - приближение/отдаление" << endl;
    cout << "  WASD - перемещение камеры" << endl;
    cout << "  R - сброс масштаба и положения" << endl;
    cout << "Нажмите Ctrl+C для выхода" << endl;
    Sleep(3000);

    int lastWidth = 0, lastHeight = 0;
    vector<Star> stars;
    vector<Planet> planets;
    vector<Comet> comets;
    vector<Starship> starships;
    int centerX = 0, centerY = 0;
    int frame = 0;

    while (true) {
        try {
            handleInput();

            int consoleWidth, consoleHeight;
            getConsoleSize(consoleWidth, consoleHeight);

            consoleWidth = max(consoleWidth, 40);
            consoleHeight = max(consoleHeight, 20);

            centerX = consoleWidth / 2;
            centerY = consoleHeight / 2;

            if (consoleWidth != lastWidth || consoleHeight != lastHeight) {
                system("cls");
                stars = createStarfield(consoleWidth, consoleHeight);
                planets = createRandomPlanets(centerX, centerY, consoleWidth, consoleHeight);
                comets = createComets(consoleWidth, consoleHeight);
                starships = createStarships(consoleWidth, consoleHeight);
                lastWidth = consoleWidth;
                lastHeight = consoleHeight;
            }

            updatePlanets(planets, centerX, centerY, consoleWidth, consoleHeight);
            updateComets(comets, consoleWidth, consoleHeight);
            updateStarships(starships, consoleWidth, consoleHeight);

            system("cls");

            drawStars(stars, consoleWidth, consoleHeight, frame);
            drawBlackHole(centerX, centerY, consoleWidth, consoleHeight, frame);
            drawPlanets(planets, consoleWidth, consoleHeight);
            drawComets(comets, consoleWidth, consoleHeight);
            drawStarships(starships, consoleWidth, consoleHeight);

            setCursorPosition(0, consoleHeight - 1);
            setColor(7);
            cout << "Масштаб: " << scale << "x";

            setCursorPosition(consoleWidth - 30, consoleHeight - 1);
            cout << "Кадр: " << frame;

            frame++;
            Sleep(50);
        }
        catch (const exception& e) {
            system("cls");
            cout << "Произошла ошибка: " << e.what() << endl;
            cout << "Перезапускаем симуляцию..." << endl;
            Sleep(2000);

            lastWidth = 0;
            lastHeight = 0;
            stars.clear();
            planets.clear();
            comets.clear();
            starships.clear();
            scale = 1.0;
            offsetX = 0;
            offsetY = 0;
        }
    }

    return 0;
}
