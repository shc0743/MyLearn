#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <emscripten.h>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

// 方向枚举
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 蛇的段结构
struct SnakeSegment {
    int x, y;
};

class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();
    
    bool init();
    void run();
    void mainLoop();
    void close();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    
    std::vector<SnakeSegment> snake;
    Direction direction;
    int foodX, foodY;
    int score;
    bool running;
    bool gameOver;
    Uint32 lastUpdate;
    
    void handleEvents();
    void update();
    void render();
    void spawnFood();
    void resetGame();
    void renderText(const std::string& text, int x, int y, SDL_Color color);
};

SnakeGame::SnakeGame() 
    : window(nullptr), renderer(nullptr), font(nullptr), 
      direction(RIGHT), score(0), running(true), gameOver(false), lastUpdate(0) {
    
    // 初始化蛇身
    snake.push_back({GRID_WIDTH / 2, GRID_HEIGHT / 2});
    snake.push_back({GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2});
    snake.push_back({GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2});
    
    spawnFood();
}

SnakeGame::~SnakeGame() {
    close();
}

bool SnakeGame::init() {
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL初始化失败: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 创建窗口 - 在Web环境中使用不同的标志
    window = SDL_CreateWindow("贪吃蛇游戏", 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, 
                              SCREEN_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 创建渲染器 - 在Web环境中使用软件渲染
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        // 如果软件渲染失败，尝试其他选项
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "渲染器创建失败: " << SDL_GetError() << std::endl;
            return false;
        }
    }
    
    // 初始化SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF初始化失败: " << TTF_GetError() << std::endl;
        // 继续运行，只是没有文本
    } else {
        // 在Web环境中使用预加载的字体
        font = TTF_OpenFont("/fonts/DroidSans.ttf", 16);
        if (!font) {
            std::cout << "无法加载字体，将使用图形显示" << std::endl;
        } else {
            std::cout << "成功加载字体" << std::endl;
        }
    }
    
    return true;
}

void SnakeGame::mainLoop() {
    handleEvents();
    
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - lastUpdate;
    
    if (elapsedTime > 100) { // 控制游戏速度
        if (!gameOver) {
            update();
        }
        lastUpdate = currentTime;
    }
    
    render();
}

void SnakeGame::run() {
    // 在Web环境中使用emscripten_set_main_loop
    emscripten_set_main_loop_arg([](void* arg) {
        static_cast<SnakeGame*>(arg)->mainLoop();
    }, this, 0, 1);
}

void SnakeGame::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            emscripten_cancel_main_loop();
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_w:
                    if (direction != DOWN) direction = UP;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    if (direction != UP) direction = DOWN;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    if (direction != RIGHT) direction = LEFT;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    if (direction != LEFT) direction = RIGHT;
                    break;
                case SDLK_r:
                    if (gameOver) resetGame();
                    break;
                case SDLK_ESCAPE:
                case SDLK_q:
                    running = false;
                    emscripten_cancel_main_loop();
                    break;
            }
        }
    }
}

void SnakeGame::update() {
    // 保存蛇头位置
    int headX = snake[0].x;
    int headY = snake[0].y;
    
    // 根据方向移动蛇头
    switch (direction) {
        case UP:
            headY--;
            break;
        case DOWN:
            headY++;
            break;
        case LEFT:
            headX--;
            break;
        case RIGHT:
            headX++;
            break;
    }
    
    // 检查边界碰撞
    if (headX < 0 || headX >= GRID_WIDTH || headY < 0 || headY >= GRID_HEIGHT) {
        gameOver = true;
        return;
    }
    
    // 检查自身碰撞
    for (size_t i = 1; i < snake.size(); i++) {
        if (headX == snake[i].x && headY == snake[i].y) {
            gameOver = true;
            return;
        }
    }
    
    // 移动蛇身
    SnakeSegment newHead = {headX, headY};
    snake.insert(snake.begin(), newHead);
    
    // 检查是否吃到食物
    if (headX == foodX && headY == foodY) {
        score += 10;
        spawnFood();
    } else {
        // 如果没有吃到食物，移除尾部
        snake.pop_back();
    }
}

void SnakeGame::render() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // 绘制网格线
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    for (int x = 0; x < SCREEN_WIDTH; x += GRID_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }
    for (int y = 0; y < SCREEN_HEIGHT; y += GRID_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
    
    // 绘制食物
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_Rect foodRect = {foodX * GRID_SIZE + 1, foodY * GRID_SIZE + 1, GRID_SIZE - 2, GRID_SIZE - 2};
    SDL_RenderFillRect(renderer, &foodRect);
    
    // 绘制蛇
    for (size_t i = 0; i < snake.size(); i++) {
        SDL_Rect segmentRect = {
            snake[i].x * GRID_SIZE + 1, 
            snake[i].y * GRID_SIZE + 1, 
            GRID_SIZE - 2, 
            GRID_SIZE - 2
        };
        
        // 蛇头用不同颜色
        if (i == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }
        
        SDL_RenderFillRect(renderer, &segmentRect);
    }
    
    // 显示分数
    if (font) {
        SDL_Color white = {255, 255, 255, 255};
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText, 10, 10, white);
        
        if (gameOver) {
            SDL_Color red = {255, 0, 0, 255};
            renderText("Game Over!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 30, red);
            renderText("Press R to restart", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2, white);
        }
    }
    
    SDL_RenderPresent(renderer);
}

void SnakeGame::spawnFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> disY(0, GRID_HEIGHT - 1);
    
    bool onSnake;
    do {
        onSnake = false;
        foodX = disX(gen);
        foodY = disY(gen);
        
        for (const auto& segment : snake) {
            if (foodX == segment.x && foodY == segment.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void SnakeGame::resetGame() {
    snake.clear();
    snake.push_back({GRID_WIDTH / 2, GRID_HEIGHT / 2});
    snake.push_back({GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2});
    snake.push_back({GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2});
    
    direction = RIGHT;
    score = 0;
    gameOver = false;
    
    spawnFood();
}

void SnakeGame::renderText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SnakeGame::close() {
    if (font) {
        TTF_CloseFont(font);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    TTF_Quit();
    SDL_Quit();
}

// 全局游戏实例
SnakeGame* game = nullptr;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void startGame() {
        game = new SnakeGame();
        if (game->init()) {
            game->run();
        }
    }
    
    EMSCRIPTEN_KEEPALIVE
    void stopGame() {
        if (game) {
            delete game;
            game = nullptr;
        }
    }
}

int main() {
    startGame();
    return 0;
}