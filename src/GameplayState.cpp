#include "GameplayState.h"

#include <string>
using namespace std;

void GameplayState::Initialize()
{
    this->AddMass(Vector2D(0.0f, 0.0f), 100.0f, 10.0f);
    this->AddMass(Vector2D(1.0f, 1.0f), 100.0f, 20.0f);
    this->AddMass(Vector2D(-1.0f, -1.0f), 100.0f, 15.0f);
    
}

void GameplayState::AddMass(Vector2D position, float mass, float radius)
{
    if(this->numMasses >= kMaxMasses) return;

    this->numMasses++;
    this->masses[numMasses].SetPosition(position);
    this->masses[numMasses].SetRadius(radius);
    this->masses[numMasses].SetMass(mass);
    this->totalMass += mass;
}

void GameplayState::Cleanup()
{
}

void GameplayState::SetPerspective()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f / this->zoomFactor, (float)viewport[2] / (float)viewport[3], 0.1, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
}


// This might be very messy without a 3d vector class. We need to write one, but I 
// recommend keeping it separate from the 3d vector class because 99% of the time
// we know the z component of what we're using and it's always the same value. There's
// no reason to use a 3-dimensional vector there and store an extra float value that may
// never be used and may never change.
// 
// However, here a 3d vector would be useful because we actually are dealing with depth to
// intersect with the plane on the Y axis...
//
// CURRENT FUNCTION STATUS: BROKEN
Vector2D GameplayState::GetOGLCoordinates(float x, float y)
{
    GLint viewport[4];
    GLdouble modelView[16];
    GLdouble projection[16];
    GLfloat winX, winY;
    GLfloat newDepth;
    GLdouble x1, y1, z1, x2, y2, z2;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - y;


    //glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &newDepth);


    // We unProject twice with z = 1 and z = 0 then intersect the line segment created by these
    // two points with the Y-plane. The point of intersection is our position.

    // Near plane
    gluUnProject(winX, winY,  0, modelView, projection, viewport, &x1, &y1, &z1);

    // Far plane
    gluUnProject(winX, winY,  1, modelView, projection, viewport, &x2, &y2, &z2);



    return Vector2D((x2 - x1) * (z2 / z1) , (y2 - y1) * (z2/z1));
}

bool GameplayState::HandleEvents()
{
    SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
        switch(event.type)
        {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {               
            case SDLK_DOWN:
                if((this->zoomFactor - 0.1f) <= 0)
                {
                    this->zoomFactor = 0;
                }
                else
                {
                    this->zoomFactor -= 0.1f;
                }
                this->SetPerspective();
                break;
            case SDLK_UP:
                if((this->zoomFactor + 0.1f) >= 1.0f)
                {
                    this->zoomFactor = 1.0f;
                }
                else
                {
                    this->zoomFactor += 0.1f;
                }
                this->SetPerspective();
                break;
            case SDLK_ESCAPE: 
                return false;

            }
        case SDL_MOUSEBUTTONDOWN:
            this->AddMass(this->GetOGLCoordinates((float)event.button.x, (float)event.button.y), 1000.0f, 10.0f);
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