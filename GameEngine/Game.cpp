#include "Game.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "AnimSpriteComponent.h"
#include "Timer.h"
#include "Assets.h"
#include "BackgroundSpriteComponent.h"
#include "RectangleComponent.h"
#include "Ball.h"
#include "Paddle.h"
#include <stdlib.h>
#include <time.h>

bool Game::initialize()
{
	bool isWindowInit = window.initialize();
	bool isRendererInit = renderer.initialize(window);
	return isWindowInit && isRendererInit; // Return bool && bool && bool ...to detect error
}

void Game::load()
{
	// Load textures
	Assets::loadTexture(renderer, "Res\\Farback01.png", "Farback01");
	Assets::loadTexture(renderer, "Res\\Farback02.png", "Farback02");
	Assets::loadTexture(renderer, "Res\\Stars.png", "Stars");
	Assets::loadTexture(renderer, "Res\\Apple01.png", "Apple01");
	Assets::loadTexture(renderer, "Res\\Apple02.png", "Apple02");
	Assets::loadTexture(renderer, "Res\\Apple03.png", "Apple03");
	Assets::loadTexture(renderer, "Res\\Apple04.png", "Apple04");
	Assets::loadTexture(renderer, "Res\\Wood01.png", "Wood01");
	Assets::loadTexture(renderer, "Res\\Wood02.png", "Wood02");
	Assets::loadTexture(renderer, "Res\\Wood03.png", "Wood03");
	Assets::loadTexture(renderer, "Res\\Wood04.png", "Wood04");


	// Background
	// Create the "far back" background
	vector<Texture*> bgTexsFar{
		&Assets::getTexture("Farback01"),
			& Assets::getTexture("Farback02")
	};
	Actor* bgFar = new Actor();
	BackgroundSpriteComponent* bgSpritesFar = new BackgroundSpriteComponent(bgFar, bgTexsFar);
	bgSpritesFar->setScrollSpeed(-100.0f);

	// Create the closer background
	Actor* bgClose = new Actor();
	std::vector<Texture*> bgTexsClose {
		&Assets::getTexture("Stars"),
			& Assets::getTexture("Stars")
	};
	BackgroundSpriteComponent* bgSpritesClose = new BackgroundSpriteComponent(bgClose, bgTexsClose);
	bgSpritesClose->setScrollSpeed(-200.0f);


	// Cr�ation de la balle
	new Ball((int)Assets::getTexture("Apple01").getWidth(), (int)Assets::getTexture("Apple01").getHeight());

	// Animated sprite
	vector<Texture*> animTextures {
		&Assets::getTexture("Apple01"),
			& Assets::getTexture("Apple02"),
			& Assets::getTexture("Apple03"),
			& Assets::getTexture("Apple04"),
	};
	AnimSpriteComponent* animatedSprite = new AnimSpriteComponent(ball, animTextures);
	ball->setPosition(Vector2{ 400, 400 });
	int lives = 5;
	ball->setLives(lives);


	// Cr�ation de la raquette
	new Paddle((int)Assets::getTexture("Wood04").getWidth(), (int)Assets::getTexture("Wood04").getHeight());
	SpriteComponent* spritePaddle = new SpriteComponent(paddle, Assets::getTexture("Wood04"));
	paddle->setPosition(Vector2{ 450, 700 });


	// Cr�ation des briques
	srand(time(NULL));
	const int horizontalBricks = 8;
	const int verticalBricks = 5;
	const int brickSizeX = 128;
	const int brickSizeY = 32;
	int offsetX = 1;
	int offsetY = 1;
	float xPos = 0;

	for (int i = 0; i < horizontalBricks; ++i)
	{
		float yPos = 100;
		for (int i = 0; i < verticalBricks; ++i) {
			Brick* brick = new Brick(Assets::getTexture("Wood01").getWidth(), (int)Assets::getTexture("Wood01").getHeight());
			brick->setPosition(Vector2{ xPos, yPos });

			int randWood = rand() % 3 + 1;
			if (randWood == 1) SpriteComponent* spriteBrick = new SpriteComponent(brick, Assets::getTexture("Wood01"));
			else if (randWood == 2) SpriteComponent* spriteBrick = new SpriteComponent(brick, Assets::getTexture("Wood02"));
			else SpriteComponent* spriteBrick = new SpriteComponent(brick, Assets::getTexture("Wood03"));
			yPos += brickSizeY + offsetY;
		}
		xPos += brickSizeX + offsetX;
	}


	// Cr�ation des vies

	float liveSizeX = Assets::getTexture("Apple01").getWidth();
	float liveSizeY = Assets::getTexture("Apple01").getHeight();
	xPos = 20;
	float yPos = 730;
	offsetX = 10;

	for (int i = 0; i < lives; ++i) {
		Live* live = new Live(Assets::getTexture("Apple01").getWidth(), (int)Assets::getTexture("Apple01").getHeight());
		live->setPosition(Vector2{ xPos, yPos });
		SpriteComponent* spriteBrick = new SpriteComponent(live, Assets::getTexture("Apple01"));
		xPos += liveSizeX + offsetX;
	}
}

vector<Brick*>& Game::getBricks()
{
	return bricks;
}

void Game::addBrick(Brick* brick)
{
	bricks.emplace_back(brick);
}

void Game::removeBrick(Brick* brick)
{
	auto iter = std::find(begin(bricks), end(bricks), brick);
	if (iter != bricks.end())
	{
		bricks.erase(iter);
	}
}

Paddle* Game::getPaddle()
{
	return paddle;
}

void Game::setPaddle(Paddle* paddleP)
{
	paddle = paddleP;
}

Ball* Game::getBall()
{
	return ball;
}

void Game::setBall(Ball* ballP)
{
	ball = ballP;
}

vector<Live*>& Game::getLives()
{
	return lives;
}

void Game::addLive(Live* live)
{
	lives.emplace_back(live);
}

void Game::removeLive(Live* live)
{
	auto iter = std::find(begin(lives), end(lives), live);
	if (iter != lives.end())
	{
		lives.erase(iter);
	}
}

void Game::endGame()
{
	isRunning = false;
	close();
}

void Game::processInput()
{
	// SDL Event
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		}
	}
	// Keyboard state
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	// Escape: quit game
	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}
	// Actor input
	isUpdatingActors = true;
	for (auto actor : actors)
	{
		actor->processInput(keyboardState);
	}
	isUpdatingActors = false;
}

void Game::update(float dt)
{
	// Update actors 
	isUpdatingActors = true;
	for (auto actor : actors)
	{
		actor->update(dt);
	}
	isUpdatingActors = false;

	// Move pending actors to actors
	for (auto pendingActor : pendingActors)
	{
		actors.emplace_back(pendingActor);
	}
	pendingActors.clear();

	// Delete dead actors
	vector<Actor*> deadActors;
	for (auto actor : actors)
	{
		if (actor->getState() == Actor::ActorState::Dead)
		{
			deadActors.emplace_back(actor);
		}
	}
	for (auto deadActor : deadActors)
	{
		delete deadActor;
	}
}

void Game::render()
{
	renderer.beginDraw();
	renderer.draw();
	renderer.endDraw();
}

void Game::loop()
{
	Timer timer;
	float dt = 0;
	while (isRunning)
	{
		float dt = timer.computeDeltaTime() / 1000.0f;
		processInput();
		update(dt);
		render();
		timer.delayTime();
	}
}

void Game::unload()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!actors.empty())
	{
		delete actors.back();
	}

	// Resources
	Assets::clear();
}

void Game::close()
{
	renderer.close();
	window.close();
	SDL_Quit();
}

void Game::addActor(Actor* actor)
{
	if (isUpdatingActors)
	{
		pendingActors.emplace_back(actor);
	}
	else
	{
		actors.emplace_back(actor);
	}
}

void Game::removeActor(Actor* actor)
{
	// Erase actor from the two vectors
	auto iter = std::find(begin(pendingActors), end(pendingActors), actor);
	if (iter != end(pendingActors))
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, end(pendingActors) - 1);
		pendingActors.pop_back();
	}
	iter = std::find(begin(actors), end(actors), actor);
	if (iter != end(actors))
	{
		std::iter_swap(iter, end(actors) - 1);
		actors.pop_back();
	}
}