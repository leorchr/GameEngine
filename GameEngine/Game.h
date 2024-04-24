#pragma once
#include "Actor.h"
#include "CircleActor.h"
#include "CircleActor.h"
#include "ControllerActor.h"
#include "Fruit.h"
#include "InputSystem.h"
#include "Physics.h"
#include "Renderer.h"
#include "Vector2.h"
#include "Window.h"
#include <vector>
using std::vector;

class Game
{
public:
	static Game& instance()
	{
		static Game inst;
		return inst;
	}

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;

private:
	Game() : isRunning(true), isUpdatingActors(false){}

public:
	bool initialize();
	void load();
	void loop();
	void unload();
	void close();

	void addActor(Actor* actor);
	void removeActor(Actor* actor);

	Renderer& getRenderer() { return renderer; }
	Physics& getPhysics() { return physics; }

	//Game specific
	void addCircle(CircleActor* circle);
	void removeCircle(CircleActor* circle);
	void spawnFruit(Fruit fruitP, Vector2 posP);
	static vector<Fruit> fruits;
	void gameOver();
	Fruit getNextFruit() { return nextFruit; }
	void setNextFruit();

private:
	void processInput();
	void update(float dt);
	void render();

	bool isRunning;
	Window window;
	Renderer renderer;
	Physics physics;
	InputSystem inputSystem;

	bool isUpdatingActors;
	vector<Actor*> actors;
	vector<Actor*> pendingActors;

	//Simulation variables
	ControllerActor* controller;
	vector<CircleActor*> circles;
	float timeBetweenSpawn;
	float baseTimeBetweenSpawn;
	float spawnSpeed;
	Vector2 spawnPos;

	Fruit nextFruit;
	Actor* nextFruitDisplay;
};