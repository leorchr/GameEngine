#include "Game.h"
#include "Actor.h"
#include "Timer.h"
#include "Assets.h"
#include "Actor.h"
#include "Rectangle.h"
#include "CircleActor.h"
#include "Color.h"
#include <algorithm>

vector<Fruit>Game::fruits = { Fruits::cherry, Fruits::strawberry, Fruits::dekopon, Fruits::orange, Fruits::apple };

bool Game::initialize()
{
	bool isWindowInit = window.initialize();
	bool isRendererInit = renderer.initialize(window);
	bool isInputInit = inputSystem.initialize();
	return isWindowInit && isRendererInit && isInputInit; // Return bool && bool && bool ...to detect error
}

void Game::load()
{

	Assets::loadTexture(renderer, "Res\\Cloud.png", "Cloud");

	controller = new ControllerActor(200.0f, 100.0f);
	baseTimeBetweenSpawn = 1.0f;
	timeBetweenSpawn = 1.0f;
	spawnSpeed = 30.0f;
	getPhysics().setLeftBorder(WINDOW_WIDTH/2 - 350.0f);
	getPhysics().setRightBorder(WINDOW_WIDTH / 2 + 350.0f);
	getPhysics().setBottomBorder(WINDOW_HEIGHT - 80.0f);
	getPhysics().setTopBorder(80.0f);
}

void Game::spawnFruit(Fruit fruitP, Vector2 posP)
{
	CircleActor* circle = new CircleActor(posP, fruitP);
	addCircle(circle);
}

void Game::processInput()
{
	inputSystem.preUpdate();

	// SDL Event
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		isRunning = inputSystem.processEvent(event);
	}

	inputSystem.update();
	const InputState& input = inputSystem.getInputState();

	// Escape: quit game
	if (input.keyboard.getKeyState(SDL_SCANCODE_ESCAPE) == ButtonState::Released)
	{
		isRunning = false;
	}

	// Actor input
	isUpdatingActors = true;
	for (auto actor : actors)
	{
		actor->processInput(input);
	}
	isUpdatingActors = false;
}

void Game::update(float dt)
{
	physics.computePhysics(dt);

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
	inputSystem.close();
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

void Game::addCircle(CircleActor* circle)
{
	circles.emplace_back(circle);
}

void Game::removeCircle(CircleActor* circle)
{
	auto iter = std::find(begin(circles), end(circles), circle);
	circles.erase(iter);
}