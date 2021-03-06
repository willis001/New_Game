#include "stdafx.h"
#include "State.h"

State::State(StateData* state_data)
{
	this->stateData = state_data;
	this->window = state_data->window;
	this->supportedKeys = state_data->supportedKeys;
	this->states = state_data->states;
	this->quit = false;
	this->paused = false;
	this->keytime = 0.f;
	this->keytime1 = 0.f;
	this->keytime2 = 0.f;
	this->keytime3 = 0.f;
	this->keytimeMax = 20.f;
	this->gridSize = state_data->gridSize;
	this->inMainMenuState = true;
}

State::~State()
{

}

const bool& State::getInMainMenuState() const
{
	return this->inMainMenuState;
}

const bool& State::getQuit() const
{
	return this->quit;
}

const bool State::getKeytime()
{
	if (this->keytime >= this->keytimeMax)
	{
		this->keytime = 0.f;
		return true;
	}

	return false;
}

const bool State::getKeytime1()
{
	if (this->keytime1 >= this->keytimeMax)
	{
		this->keytime1 = 0.f;
		return true;
	}

	return false;
}

const bool State::getKeytime2()
{
	if (this->keytime2 >= this->keytimeMax)
	{
		this->keytime2 = 0.f;
		return true;
	}

	return false;
}

const bool State::getKeytime3()
{
	if (this->keytime3 >= this->keytimeMax)
	{
		this->keytime3 = 0.f;
		return true;
	}

	return false;
}

void State::nowInMainMenuState()
{
	this->inMainMenuState = true;
}

void State::nowOutMainMenuState()
{
	this->inMainMenuState = false;
}

void State::endState()
{
	this->quit = true;
}

void State::pauseState()
{
	this->paused = true;
}

void State::unpauseState()
{
	this->paused = false;
}

void State::updateMousePosition(sf::View* view)
{
	this->mousePosScreen = sf::Mouse::getPosition();
	this->mousePosWindow = sf::Mouse::getPosition(*this->window);

	if(view)
		this->window->setView(*view);

	this->mousePosView = this->window->mapPixelToCoords(sf::Mouse::getPosition(*this->window));
	this->mousePosGrid = 
		sf::Vector2i(
			static_cast<int>(this->mousePosView.x) / static_cast<int>(this->gridSize),
			static_cast<int>(this->mousePosView.y) / static_cast<int>(this->gridSize)
	);

	this->window->setView(this->window->getDefaultView());
}

void State::updateKeytime(const float& dt)
{
	if (this->keytime < this->keytimeMax)
		this->keytime += 100.f * dt;
	if (this->keytime1 < this->keytimeMax)
		this->keytime1 += 100.f * dt;
	if (this->keytime2 < this->keytimeMax)
		this->keytime2 += 150.f * dt;
	if (this->keytime3 < this->keytimeMax)
		this->keytime3 += 200.f * dt;
}
