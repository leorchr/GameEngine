#include "Game.h"
#include "Actor.h"
#include "Timer.h"
#include "Assets.h"
#include "Actor.h"
#include "Rectangle.h"
#include <algorithm>

bool Game::initialize()
{
	bool isWindowInit = window.initialize();
	bool isRendererInit = renderer.initialize(window);
	return isWindowInit && isRendererInit; // Return bool && bool && bool ...to detect error
}

void Game::load()
{
	Assets::loadTexture(renderer, "Res\\Cloud.png", "Cloud");

	controller = new ControllerActor(200.0f);
	time = 0.0f;
	baseTimeBetweenSpawn = 1.0f;
	timeBetweenSpawn = 1.0f;
	spawnSpeed = 30.0f;
	maxCircles = 50;
	minRadius = 50.0f;
	maxRadius = 80.0f;
	getPhysics().setLeftBorder(WINDOW_WIDTH/2 - 350.0f);
	getPhysics().setRightBorder(WINDOW_WIDTH / 2 + 350.0f);
	getPhysics().setBottomBorder(WINDOW_HEIGHT - 80.0f);
	getPhysics().setTopBorder(80.0f);
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
		/*case SDL_MOUSEBUTTONDOWN:
			int mouseX, mouseY;
			Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
			new CircleActor(Vector2(mouseX, mouseY), 15, Vector3((rand() % 155 + 100), (rand() % 155 + 100), (rand() % 155 + 100)));
			break;*/
		}
	}
	// Keyboard state
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	// Escape: quit game
	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}

	if(SDL_BUTTON_LEFT)
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
	renderer.drawRect(Rectangle(controller->getPosition().x, controller->getPosition().y, 200.0f, 200.0f));

	// Simulate physics
	spawnPos = controller->getPosition();
	time += dt;
	if (maxCircles > 0 && timeBetweenSpawn < 0) {
		float range = maxRadius - minRadius;
		CircleActor* circle = new CircleActor(spawnPos, (rand() % (int)range + minRadius), Vector3(0.0f,0.0f,255.0f));

		const float angle = sin(time) + Maths::pi * 0.5f;

		getPhysics().setObjectVelocity(*circle->getMoveComponent(), spawnSpeed * Vector2(cos(angle), sin(angle)));
		addCircle(circle);
		timeBetweenSpawn = baseTimeBetweenSpawn;
		maxCircles--;
	}
	else timeBetweenSpawn -= dt;
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