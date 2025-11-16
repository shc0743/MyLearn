#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <random>
#include <string>

const int SCREEN_WIDTH = 400;  // 减小窗口尺寸
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
    
    void handleEvents();
    void update();
    void render();
    void spawnFood();
    void resetGame();
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void drawScoreWithoutFont();
};

SnakeGame::SnakeGame() 
    : window(nullptr), renderer(nullptr), font(nullptr), 
      direction(RIGHT), score(0), running(true), gameOver(false) {
    
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
    
    // 创建窗口 - 使用可调整大小
    window = SDL_CreateWindow("贪吃蛇游戏", 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, 
                              SCREEN_HEIGHT, 
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "渲染器创建失败: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // 初始化SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF初始化失败: " << TTF_GetError() << std::endl;
        // 继续运行，只是没有文本
    } else {
        // 在Termux中尝试不同的字体路径
        const char* fontPaths[] = {
            "/system/fonts/DroidSans.ttf",
            "/system/fonts/NotoSansCJK-Regular.ttc",
            "/system/fonts/Roboto-Regular.ttf",
            "/data/data/com.termux/files/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            nullptr
        };
        
        for (int i = 0; fontPaths[i] != nullptr; i++) {
            font = TTF_OpenFont(fontPaths[i], 16); // 使用更小的字体大小
            if (font) {
                std::cout << "成功加载字体: " << fontPaths[i] << std::endl;
                break;
            }
        }
        
        if (!font) {
            std::cout << "无法加载字体，将使用图形显示分数" << std::endl;
        }
    }
    
    return true;
}

void SnakeGame::run() {
    Uint32 lastUpdate = SDL_GetTicks();
    const Uint32 frameDelay = 100; // 控制游戏速度
    
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - lastUpdate;
        
        if (elapsedTime > frameDelay) {
            handleEvents();
            if (!gameOver) {
                update();
            }
            lastUpdate = currentTime;
        }
        
        render();
        SDL_Delay(10); // 防止CPU占用过高
    }
}

void SnakeGame::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
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

void SnakeGame::drawScoreWithoutFont() {
    // 使用简单的图形方式显示分数
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // 在左上角绘制分数背景
    SDL_Rect scoreBg = {5, 5, 80, 25};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &scoreBg);
    
    // 绘制分数文本的简单表示
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    std::string scoreStr = std::to_string(score);
    
    // 简单的数字显示（使用线条）
    for (int i = 0; i < scoreStr.length(); i++) {
        int digit = scoreStr[i] - '0';
        // 这里可以添加简单的数字绘制逻辑，但为了简单，我们直接显示数字字符串的位置
    }
    
    // 绘制边框
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &scoreBg);
}

void SnakeGame::render() {
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // 绘制网格线（更细的线）
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
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // 绿色头部
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // 深绿色身体
        }
        
        SDL_RenderFillRect(renderer, &segmentRect);
    }
    
    // 显示分数
    if (font) {
        SDL_Color white = {255, 255, 255, 255};
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText, 10, 10, white);
    } else {
        drawScoreWithoutFont();
    }
    
    // 游戏结束显示
    if (gameOver) {
        if (font) {
            SDL_Color red = {255, 0, 0, 255};
            SDL_Color white = {255, 255, 255, 255};
            
            renderText("Game Over!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 30, red);
            renderText("Press R to restart", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2, white);
            renderText("Press Q to quit", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 30, white);
        } else {
            // 没有字体时的游戏结束显示
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect gameOverBg = {SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 40, 160, 80};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_RenderFillRect(renderer, &gameOverBg);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &gameOverBg);
        }
    }
    
    // 更新屏幕
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
        
        // 确保食物不会生成在蛇身上
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
    if (!surface) {
        return;
    }
    
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
        font = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    SnakeGame game;
    
    if (!game.init()) {
        std::cerr << "游戏初始化失败!" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}