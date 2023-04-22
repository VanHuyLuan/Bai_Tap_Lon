#pragma once
#ifndef ALGOTHRM_H
#define ALGOTHRM_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include<string>
#include<queue>

enum Tile {
    NUM_TILE_TYPES = 15, // loai o
    COVERED, // Ô cờ chưa mở
    EMPTY,   // Ô trống
    ONE,     // Ô số 1
    TWO,     // Ô số 2
    THREE,   // Ô số 3
    FOUR,    // Ô số 4
    FIVE,    // Ô số 5
    SIX,     // Ô số 6
    SEVEN,   // Ô số 7
    EIGHT,   // Ô số 8
    FLAGGED, // Ô đã đánh dấu
    BOOM     // Ô có boom
};

// Khai báo các texture cho từng loại ô trên bản đồ
SDL_Texture* tileTextures[Tile::NUM_TILE_TYPES];

// Định nghĩa kích thước ảnh và kích thước mảng 2 chiều
const int TILE_SIZE = 32;
const int MAP_WIDTH = 30;
const int MAP_HEIGHT = 20;
const int WIDTH = TILE_SIZE * MAP_WIDTH;
const int HEIGHT = TILE_SIZE * MAP_HEIGHT;


// Khai báo mảng 2 chiều để lưu các ảnh
SDL_Texture* map[MAP_WIDTH][MAP_HEIGHT];
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* numTextures[9] = { nullptr };
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* boomTexture = NULL;

// Khai báo cấu trúc cho ô đang được xét
struct Cell {
    int row;
    int col;
};

// Khai báo hằng số cho các giá trị ô trên bản đồ
const int UNKNOWN = -1;
const int BLANK = 0;
const int BOOM = 9;

// Khai báo mảng 2 chiều để lưu trạng thái của các ô trên bản đồ
int gameState[MAP_WIDTH][MAP_HEIGHT];

bool opened[MAP_WIDTH][MAP_HEIGHT];
int numBooms[MAP_WIDTH][MAP_HEIGHT];

// Hàm khởi tạo SDL và tải các tài nguyên
bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Hàm tải tài nguyên
/*bool loadResources(SDL_Renderer* renderer)
{
    // Tải ảnh background
    SDL_Surface* backgroundSurface = IMG_Load("background.png");
    if (backgroundSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    // Tải ảnh các số từ 0 đến 8
    for (int i = 0; i <= 8; i++) {
        std::string fileName = "number_" + std::to_string(i) + ".png";
        SDL_Surface* numSurface = IMG_Load(fileName.c_str());
        if (numSurface == nullptr)
        {
            std::cerr << "Failed to load image: " << fileName << std::endl;
            return false;
        }
        SDL_Texture* numTexture = SDL_CreateTextureFromSurface(renderer, numSurface);
        if (numTexture == nullptr) {
            SDL_FreeSurface(numSurface);
            std::cerr << "Failed to create texture from image: " << fileName << std::endl;
            return false;
        }
        numTextures[i] = numTexture;
        SDL_FreeSurface(numSurface);
    }
    // Load boom texture
    SDL_Surface* boomSurface = IMG_Load("boom-cell.png");
    if (boomSurface == nullptr) {
        std::cerr << "Failed to load image: boom-cell.png" << std::endl;
        return false;
    }
    boomTexture = SDL_CreateTextureFromSurface(renderer, boomSurface);
    if (boomTexture == nullptr) {
        SDL_FreeSurface(boomSurface);
        std::cerr << "Failed to create texture from image: boom-cell.png" << std::endl;
        return false;
    }
    SDL_FreeSurface(boomSurface);

    return true;
}*/


// Hàm tải tài nguyên
bool loadResources(SDL_Renderer* renderer)
{
    // Tải ảnh background
    SDL_Surface* backgroundSurface = IMG_Load("background.png");
    if (backgroundSurface == nullptr) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);
    // Tải ảnh các số từ 0 đến 8
    for (int i = 0; i <= 8; i++) {
        std::string fileName = "number_" + std::to_string(i) + ".png";
        SDL_Surface* numSurface = IMG_Load(fileName.c_str());
        if (numSurface == nullptr) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
            return false;
        }
        numTextures[i] = SDL_CreateTextureFromSurface(renderer, numSurface);
        if (numTextures[i] == nullptr) {
            std::cerr << "Failed to create number texture: " << SDL_GetError() << std::endl;
            return false;
        }
        SDL_FreeSurface(numSurface);
    }

    // Tải ảnh các ô cờ
    tileTextures[Tile::COVERED] = IMG_LoadTexture(renderer, "tile_hidden.png");
    tileTextures[Tile::FLAGGED] = IMG_LoadTexture(renderer, "tile_flagged.png");
    tileTextures[Tile::BOOM] = IMG_LoadTexture(renderer, "tile_mine.png");

    // Tải ảnh ô trống và ô số
    tileTextures[Tile::EMPTY] = IMG_LoadTexture(renderer, "tile_empty.png");
    tileTextures[Tile::ONE] = numTextures[1];
    tileTextures[Tile::TWO] = numTextures[2];
    tileTextures[Tile::THREE] = numTextures[3];
    tileTextures[Tile::FOUR] = numTextures[4];
    tileTextures[Tile::FIVE] = numTextures[5];
    tileTextures[Tile::SIX] = numTextures[6];
    tileTextures[Tile::SEVEN] = numTextures[7];
    tileTextures[Tile::EIGHT] = numTextures[8];

    return true;
}


// Hàm tìm số bom xung quanh một ô
int countAdjacentBoom(int row, int col) {
    int count = 0;
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            if (r >= 0 && r < MAP_HEIGHT && c >= 0 && c < MAP_WIDTH && map[r][c] == tileTextures[Tile::BOOM]) {
                count++;
            }
        }
    }
    return count;
}


// Hàm loang để mở các ô trống
void floodFill(int row, int col) {
    if (map[row][col] != NULL) {
        return;
    }
    map[row][col] = tileTextures[Tile::EMPTY];
    // Tìm số bom xung quanh
    int numBoom = countAdjacentBoom(row, col);
    if (numBoom > 0) {
        // Hiển thị số bom xung quanh
        map[row][col] = tileTextures[numBoom];
        return;
    }
    // Loang các ô trống xung quanh
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            if (r >= 0 && r < MAP_HEIGHT && c >= 0 && c < MAP_WIDTH) {
                floodFill(r, c);
            }
        }
    }
}


// Hàm vẽ các ảnh lên màn hình
void drawMap(SDL_Renderer* renderer) {
    for (int r = 0; r < MAP_HEIGHT; r++) {
        for (int c = 0; c < MAP_WIDTH; c++) {
            SDL_Rect dstRect = { c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (map[r][c] == tileTextures[Tile::COVERED]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::COVERED], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::FLAGGED]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::FLAGGED], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::BOOM]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::BOOM], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::EMPTY]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::EMPTY], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::ONE]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::ONE], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::TWO]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::TWO], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::THREE]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::THREE], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::FOUR]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::FOUR], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::FIVE]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::FIVE], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::SIX]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::SIX], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::SEVEN]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::SEVEN], NULL, &dstRect);
            }
            else if (map[r][c] == tileTextures[Tile::EIGHT]) {
                SDL_RenderCopy(renderer, tileTextures[Tile::EIGHT], NULL, &dstRect);
            }
        }
    }
}


// Vẽ các số lượng boom xung quanh lên màn hình
void drawNumbers(SDL_Renderer* renderer) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (map[row][col] == nullptr) continue;
            // Nếu ô hiện tại chưa mở thì không hiển thị số lượng boom xung quanh
            if (!opened[row][col]) continue;

            // Nếu số lượng boom xung quanh bằng 0 thì không hiển thị gì cả
            if (numBooms[row][col] == 0) continue;

            // Lấy kích thước của số 7
            int numWidth, numHeight;
            SDL_QueryTexture(numTextures[numBooms[row][col]], NULL, NULL, &numWidth, &numHeight);

            // Tính toán vị trí của số
            int numX = col * TILE_SIZE + (TILE_SIZE - numWidth) / 2;
            int numY = row * TILE_SIZE + (TILE_SIZE - numHeight) / 2;

            // Vẽ số lên màn hìnhu
            SDL_Rect dstRect = { numX, numY, numWidth, numHeight };
            SDL_RenderCopy(renderer, numTextures[numBooms[row][col]], NULL, &dstRect);
        }
    }
}


// check xem o co boom khong
bool isBoom(int row, int col)
{
    if (map[row][col] == tileTextures[Tile::BOOM])
    {
        return true;
    }
    else return false;
}


// Loang ô trống và hiển thị số lượng boom xung quanh
void floodFill(int row, int col) {
    // Nếu ô hiện tại đã mở hoặc là boom thì không loang nữa
    if (opened[row][col] || isBoom(row, col)) {
        return;
    }
    // Đánh dấu ô hiện tại đã mở
    opened[row][col] = true;

    // Nếu số lượng boom xung quanh lớn hơn 0 thì không loang nữa
    if (numBooms[row][col] > 0) {
        return;
    }

    // Loang các ô xung quanh
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // Không loang ô hiện tại
            if (i == 0 && j == 0) continue;

            // Tính toán vị trí của ô xung quanh
            int newRow = row + i;
            int newCol = col + j;

            // Kiểm tra xem ô có nằm trong phạm vi của bản đồ không
            if (newRow < 0 || newRow >= MAP_HEIGHT || newCol < 0 || newCol >= MAP_WIDTH) continue;

            // Loang ô xung quanh
            floodFill(newRow, newCol);
        }
    }
}


void fillNumber(int row, int col)
{
    // Đếm số lượng boom xung quanh
    int numBoom = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i == row && j == col) continue; // Không tính ô hiện tại
            if (i < 0 || i >= MAP_HEIGHT || j < 0 || j >= MAP_WIDTH) continue; // Kiểm tra xem ô có nằm trong phạm vi của bản đồ không
            if (map[i][j] == tileTextures[Tile::BOOM]) numBoom++; // Nếu ô đó là boom thì tăng biến đếm
        }
    }
    // Nếu số boom xung quanh > 0 thì vẽ ảnh số tương ứng lên ô đó
    if (numBoom > 0) {
        // Lấy texture số từ mảng đã được load từ trước
        SDL_Texture* numTexture = numTextures[numBoom];
        if (numTexture == nullptr) {
            std::cerr << "Failed to get number texture: " << SDL_GetError() << std::endl;
            return;
        }
        map[row][col] = numTexture;
        numBooms[row][col] = numBoom;
    }
    else { // Nếu số boom xung quanh = 0 thì tiến hành loang ô xung quanh
        opened[row][col] = true;
        floodFill(row, col);
    }
}



#endif // !ALGOTHRM_H
