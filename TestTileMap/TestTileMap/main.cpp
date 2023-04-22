#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include<string>
#include <queue>
#include <random>
#include <iomanip>


// Định nghĩa kích thước ảnh và kích thước mảng 2 chiều
const int TILE_SIZE = 32;
const int MAP_WIDTH = 30;
const int MAP_HEIGHT = 20;
const int WIDTH = TILE_SIZE * MAP_WIDTH;
const int HEIGHT = TILE_SIZE * MAP_HEIGHT;


enum Tile {
    NUM_TILE_TYPES = 15, // loai o
    COVERED1, COVERED2, // Ô cờ chưa mở
    EMPTY1 , EMPTY2,   // Ô trống
    ONE,     // Ô số 1
    TWO,     // Ô số 2
    THREE,   // Ô số 3
    FOUR,    // Ô số 4
    FIVE,    // Ô số 5
    SIX,     // Ô số 6
    SEVEN,   // Ô số 7
    EIGHT,   // Ô số 8
    FLAGGED1, FLAGGED2 , // Ô đã đánh dấu
    BOOM     // Ô có boom
};


// Khai báo các texture cho từng loại ô trên bản đồ
SDL_Texture* tileTextures[Tile::NUM_TILE_TYPES];


// Khai báo mảng 2 chiều để lưu các ảnh
SDL_Texture* map[MAP_WIDTH][MAP_HEIGHT];
int flagmap[MAP_WIDTH][MAP_HEIGHT] = { 0 };
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* numTextures[9] = { nullptr };
SDL_Texture* mapNumbers[9];

// Khai báo biến isPaused để lưu trạng thái game
bool isPaused = false;

Uint32 covered1, covered2, covered3, covered4, flag1 , flag2, covered;

// Khởi tạo SDL và cửa sổ
bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer, int screenWidth, int screenHeight) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo cửa sổ
    window = SDL_CreateWindow("MineSweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_Image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_Image: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Hàm tải tài nguyên
bool loadResources(SDL_Renderer* renderer)
{
    
    // Tải ảnh các số từ 1 đến 5
    for (int i = 1; i <= 5; i++) {
        std::string fileName = "Maps/number_" + std::to_string(i) + ".png";
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
    tileTextures[Tile::COVERED1] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/0_0.png"));
    SDL_QueryTexture(tileTextures[Tile::COVERED1], &covered1, NULL, NULL, NULL);
    tileTextures[Tile::COVERED2] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/0_1.png"));
    SDL_QueryTexture(tileTextures[Tile::COVERED2], &covered2, NULL, NULL, NULL);
    tileTextures[Tile::FLAGGED1] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag0.png"));
    SDL_QueryTexture(tileTextures[Tile::FLAGGED1], &flag1, NULL, NULL, NULL);
    tileTextures[Tile::FLAGGED2] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag1.png"));
    SDL_QueryTexture(tileTextures[Tile::FLAGGED2], &flag2, NULL, NULL, NULL);
    tileTextures[Tile::BOOM] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom-cell.png")); 
    // Tải ảnh ô trống và ô số
    tileTextures[Tile::EMPTY1] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_0.png"));
    tileTextures[Tile::EMPTY2] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_1.png"));
    tileTextures[Tile::ONE] = numTextures[1];
    tileTextures[Tile::TWO] = numTextures[2];
    tileTextures[Tile::THREE] = numTextures[3];
    tileTextures[Tile::FOUR] = numTextures[4];
    tileTextures[Tile::FIVE] = numTextures[5];
  //  tileTextures[Tile::SIX] = numTextures[6];
   // tileTextures[Tile::SEVEN] = numTextures[7];
   // tileTextures[Tile::EIGHT] = numTextures[8];

    return true;
}


// T?o b?n d? mìn
void create_board(int m, int n, int k, bool board[MAP_WIDTH][MAP_HEIGHT]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m * n - 1);

    for (int i = 0; i < k; i++) {
        int idx = dis(gen);
        int row = idx / n;
        int col = idx % n;
        board[row][col] = true;
    }
}

// Tìm s? lu?ng mìn xung quanh ô
int count_mines(int m, int n, bool board[MAP_WIDTH][MAP_HEIGHT], int row, int col) {
    int count = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < m && j >= 0 && j < n && board[i][j]) {
                count++;
            }
        }
    }
    return count;
}


// Duy?t theo chi?u r?ng d? tìm các ô không có mìn xung quanh ô dã du?c m?
void bfs(int m, int n, bool board[MAP_WIDTH][MAP_HEIGHT], bool visited[MAP_WIDTH][MAP_HEIGHT], int row, int col) {
    std::queue<std::pair<int, int>> q;
    q.push(std::make_pair(row, col));
    visited[row][col] = true;
    while (!q.empty()) {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();
        for (int i = r - 1; i <= r + 1; i++) {
            for (int j = c - 1; j <= c + 1; j++) {
                if (i >= 0 && i < m && j >= 0 && j < n && !board[i][j] && !visited[i][j]) {
                    visited[i][j] = true;
                    if (count_mines(m, n, board, i, j) == 0) {
                        q.push(std::make_pair(i, j));
                    }
                }
            }
        }
    }
}


// load Background
bool loadBackground(SDL_Renderer* renderer, const std::string& imagePath) {
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (surface == nullptr) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (backgroundTexture == nullptr) {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);
    return true;
}

// Load các ảnh từ tệp ảnh vào mảng 2 chiều
bool loadMap(SDL_Renderer* renderer) {
    // Đường dẫn tới thư mục chứa các tệp ảnh
    const char* path = "Maps/";
    
    // background
    if (!loadBackground(renderer, "Maps/closed-cell.png")) {
        std::cerr << "Failed to load background." << std::endl;
        return 1;
    }

    // Load các ảnh vào mảng 2 chiều
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            // Tạo đường dẫn tới tệp ảnh
            std::string filename;
            if (x % 2 == 0)
            {
                if (y % 2 == 0)
                {
                    filename = path + std::to_string(0) + "_" + std::to_string(0) + ".png";
                }
                else
                {
                    filename = path + std::to_string(0) + "_" + std::to_string(1) + ".png";
                }
            }
            else
            {
                if (y % 2 == 1)
                {
                    filename = path + std::to_string(0) + "_" + std::to_string(0) + ".png";
                }
                else
                {
                    filename = path + std::to_string(0) + "_" + std::to_string(1) + ".png";
                }
            }

            // Load ảnh từ tệp ảnh
            SDL_Surface* surface = IMG_Load(filename.c_str());
            if (!surface) {
                std::cerr << "Failed to load image: " << filename << " : " << IMG_GetError() << std::endl;
                return false;
            }

            // Chuyển đổi ảnh thành texture
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                std::cerr << "Failed to create texture: " << filename << " : " << SDL_GetError() << std::endl;
                return false;
            }

            // Lưu texture vào mảng 2 chiều
            map[x][y] = texture;

            // Giải phóng surface
            SDL_FreeSurface(surface);
        }
    }

    return true;
}

// Vẽ các ảnh lên màn hình
void drawMap(SDL_Renderer* renderer) {

    // Vẽ background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Vẽ các ảnh từ mảng 2 chiều lên màn hình
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            SDL_Rect dest = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, map[x][y], NULL, &dest);
        }
    }
}

// Giải phóng các texture trong mảng 2 chiều
void cleanup() {
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            SDL_DestroyTexture(map[x][y]);
        }
    }
}

//sự kiện chuột
void handleMouseEvent(SDL_Event event ) {

    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN://nhấp chuột
        std::cout << "Mouse button down at (" << event.button.x << ", " << event.button.y << ")" << std::endl;
    case SDL_MOUSEBUTTONUP:// nhả chuột
        std::cout << "Mouse button up at (" << event.button.x << ", " << event.button.y << ")" << std::endl;
        break;
    case SDL_MOUSEMOTION:// di chuột
        std::cout << "Mouse moved to (" << event.motion.x << ", " << event.motion.y << ")" << std::endl;
        break;
    default:
        break;
    }
}


// Hàm pause game
void pauseGame(SDL_Renderer * renderer) {
    isPaused = true;
    SDL_Texture* blackTexture = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/background.png"));
    SDL_SetTextureAlphaMod(blackTexture, 204); // Đặt độ mờ của texture màu đen
    SDL_RenderCopy(renderer, blackTexture, NULL, NULL); // Vẽ texture màu đen lên toàn bộ màn hình
    SDL_Delay(100); // Dừng thời gian để tránh việc nhanh tay nhấn lại phím và gây ra lỗi
}


// Hàm resume game
void resumeGame() {
    isPaused = false;
}


int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!initSDL(window, renderer, WIDTH, HEIGHT)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }


    if (loadResources(renderer) == false)
    {
        std::cerr  << "failed !" << std::endl;
        return 1;
    }


    // Clear the screen
    SDL_RenderClear(renderer);

    // Draw the background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    if (!loadMap(renderer)) {
        std::cerr << "Failed to load map." << std::endl;
        return 1;
    }
    int k = 600;
    bool board[MAP_WIDTH][MAP_HEIGHT] = { false };
    create_board(MAP_WIDTH, MAP_HEIGHT, k, board);
    int num_opened = 0;

    // Vòng lặp chính
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            int row = -1;
            int col = -1;
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                // Tính toán vị trí của ô được chọn
                row = event.button.x / TILE_SIZE;
                col = event.button.y / TILE_SIZE;

                SDL_QueryTexture(map[row][col], &covered, NULL, NULL, NULL);

                if (event.button.button == SDL_BUTTON_LEFT)
                {

                    // kiểm tra ô đấy đã đc mở hay chưa
                    if (covered1 != covered && covered2 != covered)
                    {
                        std::cout << "This cell has already been opened! Try again2." << std::endl;
                        continue;
                    }
                    // Ki?m tra có mìn hay không
                    if (board[row][col]) {
                        std::cout << "YOU'RE DEAD!" << std::endl;
                        for (int i = 0; i < MAP_WIDTH; i++)
                        {
                            for (int j = 0; j < MAP_HEIGHT; j++)
                            {
                                if (board[i][j])
                                {
                                    if (i == row && j == col)
                                    {
                                        map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom-cell1.png"));
                                    }
                                    else
                                    {
                                        if (i % 2 == 0)
                                        {
                                            if (j % 2 == 0)
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom1.png"));
                                            }
                                            else
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom0.png"));
                                            }
                                        }
                                        else
                                        {
                                            if (j % 2 == 1)
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom1.png"));
                                            }
                                            else
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom0.png"));
                                            }
                                        }
                                    }
                                    flagmap[i][j] = 1;
                                }
                            }
                        }
                        // quit = true;
                    }

                    else {
                        // Ð?m s? lu?ng mìn xung quanh ô dã m?
                        int count = count_mines(MAP_WIDTH, MAP_HEIGHT, board, row, col);
                        if (count == 0)
                        {
                       
                            if (row % 2 == 0)
                            {
                                if (col % 2 == 0)
                                {
                                    map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_0.png"));
                                }
                                else
                                {
                                    map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_1.png"));
                                }
                            }
                            else
                            {
                                if (col % 2 == 1)
                                {
                                    map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_0.png"));
                                }
                                else
                                {
                                    map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_1.png"));
                                }
                            }
                            flagmap[row][col] = 1;
                        }
                        else
                        {
                            map[row][col] = numTextures[count];
                            flagmap[row][col] = 1;
                        }
                        num_opened++;

                        // N?u không có mìn xung quanh, duy?t tìm các ô không có mìn xung quanh
                        if (count == 0) {
                            bool visited[MAP_WIDTH][MAP_HEIGHT] = { false };
                            bfs(MAP_WIDTH, MAP_HEIGHT, board, visited, row, col);
                            for (int i = 0; i < MAP_WIDTH; i++) {
                                for (int j = 0; j < MAP_HEIGHT; j++) {
                                    if (visited[i][j]) {
                                        int count = count_mines(MAP_WIDTH, MAP_HEIGHT, board, i, j);
                                        if (count == 0)
                                        {
                                            if (i % 2 == 0)
                                            {
                                                if (j % 2 == 0)
                                                {
                                                    map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_0.png"));
                                                }
                                                else
                                                {
                                                    map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_1.png"));
                                                }
                                            }
                                            else
                                            {
                                                if (j % 2 == 1)
                                                {
                                                    map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_0.png"));
                                                }
                                                else
                                                {
                                                    map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/1_1.png"));
                                                }
                                            }
                                            flagmap[i][j] = 1;
                                        }
                                        else
                                        {
                                            map[i][j] = numTextures[count];
                                            flagmap[i][j] = 1;
                                        }
                                        num_opened++;
                                    }
                                }
                            }
                        }

                        // Ki?m tra dã m? h?t các ô không có mìn chua
                        if (num_opened == MAP_WIDTH * MAP_HEIGHT - k) {
                            std::cout << "YOU WIN!" << std::endl;
                            for (int i = 0; i < MAP_WIDTH; i++)
                            {
                                for (int j = 0; j < MAP_HEIGHT; j++)
                                {
                                    if (board[i][j])
                                    {
                                        if (i % 2 == 0)
                                        {
                                            if (j % 2 == 0)
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom1.png"));
                                            }
                                            else
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom0.png"));
                                            }
                                        }
                                        else
                                        {
                                            if (j % 2 == 1)
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom1.png"));
                                            }
                                            else
                                            {
                                                map[i][j] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/boom0.png"));
                                            }
                                        }
                                        flagmap[row][col] = true;
                                           
                                        
                                    }
                                }
                            }
                            //quit = true;
                        }
                    }
                }

                else
                {
                    if (flagmap[row][col] == 0)
                    {
                        std::cout << "no" << std::endl;
                        if (row % 2 == 0)
                        {
                            if (col % 2 == 0)
                            {
                                map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag0.png"));
                            }
                            else
                            {
                                map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag1.png"));
                            }
                        }
                        else
                        {
                            if (col % 2 == 1)
                            {
                                map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag0.png"));
                            }
                            else
                            {
                                map[row][col] = SDL_CreateTextureFromSurface(renderer, IMG_Load("Maps/flag1.png"));
                            }
                        }
                        flagmap[row][col] = 2;
                    }
                    else if(flagmap[row][col] == 2)
                    {
                        std::cout << "yes" << std::endl;
                        if (row % 2 == 0)
                        {
                            if (col % 2 == 0)
                            {
                                map[row][col] = tileTextures[Tile::COVERED1];
                            }
                            else
                            {
                                map[row][col] = tileTextures[Tile::COVERED2];
                            }
                        }
                        else
                        {
                            if (col % 2 == 1)
                            {
                                map[row][col] = tileTextures[Tile::COVERED1];
                            }
                            else
                            {
                                map[row][col] = tileTextures[Tile::COVERED2];
                            }
                        }
                        flagmap[row][col] = 0;
                    }
                }

                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
                handleMouseEvent(event );
                break;
            default:
                break;
            }
        }

        // Xóa màn hình
        //SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderClear(renderer);

        // Vẽ các ảnh lên màn hình
        drawMap(renderer);

        // Hiển thị màn hình
        SDL_RenderPresent(renderer);
    }


    // Giải phóng các texture
    cleanup();
    SDL_DestroyTexture(backgroundTexture);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Đóng SDL_Image
    IMG_Quit();

    // Đóng SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}