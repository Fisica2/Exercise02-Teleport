#include "Game.h"
#include <math.h>

Game::Game() { currentPlatform = 1; }
Game::~Game(){}

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space, f1: god mode", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}
	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	//Load images
	if (!LoadImages())
		return false;

	//Init variables
	//size: 104x82
	Player.Init(120, 300, 100, 100, 5);
	Box.Init(320, 500, 100, 100, 5);
	Platform1.Init(20, 400, 300, 20, 5);
	Platform2.Init(325, 600, 300, 20, 5);
	idx_shot = 0;
	int w;
	SDL_QueryTexture(img_background, NULL, NULL, &w, NULL);
	Scene.Init(0, 0, w, WINDOW_HEIGHT, 4);
	god_mode = false;

	return true;
}
bool Game::LoadImages()
{
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Log("IMG_Init, failed to init required png support: %s\n", IMG_GetError());
		return false;
	}
	img_player = SDL_CreateTextureFromSurface(Renderer, IMG_Load("box.png"));
	if (img_player == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_box = SDL_CreateTextureFromSurface(Renderer, IMG_Load("box.png"));
	if (img_box == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_platform = SDL_CreateTextureFromSurface(Renderer, IMG_Load("wood.png"));
	if (img_platform == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	return true;
}
void Game::Release()
{
	//Release images
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_shot);
	IMG_Quit();
	
	//Clean up all SDL initialized subsystems
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT && rc.x > 20)	fx = -1;
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT && ra.x < 520)	fx = 1;


	//Logic
	int previousPlatform = currentPlatform;

	if (currentPlatform == 1 && rp.x == 300)
	{
		currentPlatform = 2;
	}
	else if (currentPlatform == 2 && rp.x == 300) currentPlatform = 1;

	//Player update

		
	if (right)
	{
		Platform1.Move(1, 0);
	}
	else
	{
		Platform1.Move(-1, 0);
		right = false;
	}

	if (currentPlatform == 1 && right)
	{
		Player.Move(1, 0);
	}
	else if (currentPlatform == 1 && !right) Player.Move(-1, 0);

	if (currentPlatform != previousPlatform)
	{
		if (right) Player.Move(0, 40);
		else Player.Move(0, -40);
	}

	if (rp.x == 600) right = false;
	if (rp.x == 20) right = true;

	return false;
}
void Game::Draw()
{

	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	//God mode uses red wireframe rectangles for physical objects

	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	//Box.GetRect(&ra.x, &ra.y, &ra.w, &ra.h);
	Platform1.GetRect(&rp.x, &rp.y, &rp.w, &rp.h);
	Platform2.GetRect(&rp2.x, &rp2.y, &rp2.w, &rp2.h);

	//Draw player

	SDL_RenderCopy(Renderer, img_player, NULL, &rc);

	SDL_RenderCopy(Renderer, img_platform, NULL, &rp);
	SDL_RenderCopy(Renderer, img_platform, NULL, &rp2);
	
	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}