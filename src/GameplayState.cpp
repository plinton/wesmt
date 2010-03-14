#include "GameplayState.h"

void GameplayState::Initialize()
{
    this->masses[0].SetPosition(Vector2D(0.0f, 0.0f));
    this->masses[0].SetRadius(10.0f);
    this->masses[0].SetMass(100);

    this->masses[1].SetPosition(Vector2D(1.0f, 1.0f));
    this->masses[1].SetRadius(20.0f);
    this->masses[1].SetMass(100);
    
    this->masses[2].SetPosition(Vector2D(-1.0f, -1.0f));
    this->masses[2].SetRadius(15.0f);
    this->masses[2].SetMass(100);
    
    
}

void GameplayState::Cleanup()
{
}

bool GameplayState::HandleEvents()
{
    SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			return false;
		}
		if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == SDLK_ESCAPE)
			{
				return false;
			}
		}
	}
    return true;
}

void GameplayState::Pause()
{
}

void GameplayState::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
    glTranslatef(0.0, 0.0, -6.0f);

    for(vector<Mass>::iterator it = this->masses.begin(); it != this->masses.end(); ++it)
    {
        it->Draw();
    }

    SDL_GL_SwapBuffers();
}

bool GameplayState::Update()
{
    if(!this->HandleEvents()) return false;

    for(vector<Mass>::iterator it = this->masses.begin(); it != this->masses.end(); ++it)
    {
        it->Update();
        for(vector<Mass>::iterator itB = this->masses.begin(); itB != this->masses.end(); ++itB){
            if(&it != &itB)
                it->ApplyGravityFrom(*itB, 1.0f);
        }
    }

    return true;
}

void GameplayState::Resume()
{
}