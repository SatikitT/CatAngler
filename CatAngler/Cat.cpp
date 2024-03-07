#include "Cat.h"
#include "Engine.h"
#include "CollisionHandler.h"
#include "FishingManager.h"
#include "TextureManager.h"
#include "TextManager.h"
#include "Input.h"
#include "Camera.h"
#include "SDL.h"

FishingManager* fish_manager;


Cat::Cat(Properties* props) : Character(props)
{
	int xPos;
	m_Flip = SDL_FLIP_NONE;

	m_RigidBody = new RigidBody();

	m_Collider = new Collider();
	m_Collider->setBuffer(-10, -10, 20, 15);
	
	m_Aimation = new Animation();
	m_Aimation->setProps(m_TextureID, 1, 5, 100);

	m_Inventory = new Inventory(10);

	fish_manager = new FishingManager(m_Inventory, &colliderVec);

	
}

int Cat::getX()
{
	Vector2D cam = Camera::GetInstance()->getPosition();
	SDL_Rect box = m_Collider->get();
	return box.x - cam.X + m_Collider->get().w/2;
}

int Cat::getY()
{
	Vector2D cam = Camera::GetInstance()->getPosition();
	SDL_Rect box = m_Collider->get();
	return box.y - cam.Y;
}

int Cat::getTX()
{
	return m_Transform->X;
}

int Cat::getTY()
{
	return m_Transform->Y;
}

Vector2D Cat::getCam()
{
	return Camera::GetInstance()->getPosition();
}


void Cat::draw()
{

	m_Aimation->draw(m_Transform->X, m_Transform->Y, m_Width, m_Height);

	if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Interact") && m_IsInteract == false || CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Door") && m_IsInteract == true || CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Shop") && m_IsInteract == true ) {
		TextureManager::GetInstance()->draw("interact_F", m_Transform->X, m_Transform->Y - 20, 32, 32);
	}

	if (!m_IsInteract) {
		FishingManager::GetInstance()->draw();
	}

	Vector2D cam = Camera::GetInstance()->getPosition();
	SDL_Rect box = m_Collider->get();
	box.x -= cam.X;
	box.y -= cam.Y;
	SDL_RenderDrawRect(Engine::GetInstance()->GetRenderer(), &box);
}

void Cat::drawInv() {
	SDL_Event event;
	Vector2D cam = Camera::GetInstance()->getPosition();
	if (m_Inventory->checkVisible()) {
		m_Inventory->render(cam.X, cam.Y);
		m_Inventory->handleMouseEvent(event);
	}
	m_Inventory->renderInventoryBar(cam.X, cam.Y + 530, m_IsUsing);

	
	SDL_SetRenderDrawColor(Engine::GetInstance()->GetRenderer(), 0, 255, 0, 10);
	SDL_Rect healthBar = { 253, 502, floor(m_Health * 141 / 10), 18};
	SDL_RenderFillRect(Engine::GetInstance()->GetRenderer(), &healthBar);

	TextureManager::GetInstance()->draw("healthbar", cam.X + 220, cam.Y + 415, 64, 64, SDL_FLIP_NONE, 3);

	TextureManager::GetInstance()->draw("badge", cam.X + 10, cam.Y - 13, 64, 64, SDL_FLIP_NONE, 1.5);
	TextureManager::GetInstance()->draw("coin", cam.X + 30, cam.Y + 20, 32, 32);

	std::stringstream strm;
	strm << m_Coin;
	TextManager::GetInstance()->renderText(strm.str().c_str(), cam.X + 60 , cam.Y + 22 , "assets/fonts/PixelifySans.ttf", 20);
}

void Cat::update(float dt)
{

	int frame = 5;
	SDL_RendererFlip m_Flip = SDL_FLIP_NONE;
	m_IsMoving = false;
	m_RigidBody->unsetForce();


	if (Input::GetInstance()->getMouseButtonUp(1) && checkX == 0 && checkY == 0 && !m_IsShopping) {
		if (Input::GetInstance()->getKeyDown(SDL_SCANCODE_W)) {
			m_IsMoving = true;
			lastDirection = 'W';
			m_RigidBody->applyForceY(SPEED * BACKWARD);
		}

		if (Input::GetInstance()->getKeyDown(SDL_SCANCODE_S)) {
			m_IsMoving = true;
			lastDirection = 'S';
			m_RigidBody->applyForceY(SPEED * FORWARD);
		}

		if (Input::GetInstance()->getKeyDown(SDL_SCANCODE_A)) {
			m_IsMoving = true;
			lastDirection = 'A';
			m_RigidBody->applyForceX(SPEED * BACKWARD);
		}

		if (Input::GetInstance()->getKeyDown(SDL_SCANCODE_D)) {
			m_IsMoving = true;
			lastDirection = 'D';
			m_RigidBody->applyForceX(SPEED * FORWARD);
		}
	}

	if (m_IsMoving) frame = 5; else frame = 1;

	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_I)) {
		m_Inventory->toggleVisibility();
	}

	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_F)) {
		if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Interact")) {
			m_IsInteract = true;
			m_Transform->X = 379;
			m_Transform->Y = 639;
		}
		if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Door")) {
			m_IsInteract = false;
			m_Transform->X = 672;
			m_Transform->Y = 347;
		}

		if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), "Shop")) {
			m_IsShopping = !m_IsShopping;
		}
	}

	if (lastDirection == 'W') {
		m_Aimation->setProps("cat_walkb", 1, frame, 100, m_Flip);
	}
	if (lastDirection == 'A') {
		m_Flip = SDL_FLIP_HORIZONTAL;
		m_Aimation->setProps("cat_walk", 1, frame, 100, m_Flip);
	}
	if (lastDirection == 'S') {
		if (m_IsMoving) m_Aimation->setProps("cat_walkf", 1, 5, 100, m_Flip); else m_Aimation->setProps("cat_idle", 1, 5, 100, m_Flip);
	}
	if (lastDirection == 'D') {
		m_Flip = SDL_FLIP_NONE;
		m_Aimation->setProps("cat_walk", 1, frame, 100, m_Flip);
	}

	std::string currentMap;
	if (m_IsInteract) {
		currentMap = "HouseCollision";
	}
	else {
		currentMap = "Collision";
	}


	m_RigidBody->update(dt);
	m_LastSafePosition.X = m_Transform->X;
	m_Transform->X += m_RigidBody->position().X;
	m_Collider->set(m_Transform->X, m_Transform->Y, 32, 32);
	if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), currentMap)) {
		m_Transform->X = m_LastSafePosition.X;
	}
		
	m_RigidBody->update(dt);
	m_LastSafePosition.Y = m_Transform->Y;
	m_Transform->Y += m_RigidBody->position().Y;
	m_Collider->set(m_Transform->X, m_Transform->Y, 32, 32);
	if (CollisionHandler::GetInstance()->mapCollision(m_Collider->get(), currentMap)) {
		m_Transform->Y = m_LastSafePosition.Y;
	}

	if (CollisionHandler::GetInstance()->checkCollisionVec(m_Collider, colliderVec)) {
		//m_Transform->X = m_LastSafePosition.X;
		//m_Transform->Y = m_LastSafePosition.Y;
	}


	m_Origin->x = m_Transform->X + m_Width / 2;
	m_Origin->y = m_Transform->Y + m_Height / 2;
	m_Aimation->update();

	fish_manager->update(dt, m_Transform->X, m_Transform->Y, &m_Health, m_Collider->get());

}

void Cat::equip() {
	
	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_1) || m_IsUsing == 0) {
		m_IsUsing = 0;
	}
	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_2) || m_IsUsing == 1) {
		m_IsUsing = 1;
	}
	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_3) || m_IsUsing == 2) {
		m_IsUsing = 2;
	}
	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_4) || m_IsUsing == 3) {
		m_IsUsing = 3;
	}
	if (Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_5) || m_IsUsing == 4) {
		m_IsUsing = 4;
	}

	if (m_Inventory->getItems().size() > m_IsUsing && !m_IsShopping && m_Inventory->getItems()[m_IsUsing] != NULL && !m_Inventory->checkVisible()) {
		m_Inventory->getItems()[m_IsUsing]->update(lastDirection, m_Transform->X + 15, m_Transform->Y, fish_manager->getEnemies(), &m_Health);
		m_Inventory->getItems()[m_IsUsing]->draw();
		if (!Input::GetInstance()->getKeyDown(SDL_NUM_SCANCODES)) {
			m_Inventory->getItems()[m_IsUsing]->use();
		}
		if (m_Inventory->getItems()[m_IsUsing]->getType() == "Rod" && !m_IsInteract) {
			checkX = m_Inventory->getItems()[m_IsUsing]->getX();
			checkY = m_Inventory->getItems()[m_IsUsing]->getY();
			fish_manager->checkFishing(checkX, checkY,"Water");
		}
		if (m_Inventory->getItems()[m_IsUsing]->getType() == "Food" && Input::GetInstance()->getKeyDownOnetime(SDL_SCANCODE_E)) {
			m_Inventory->removeItem(m_Inventory->getItems()[m_IsUsing]);
		}
	}

}

void Cat::clean()
{
	TextureManager::GetInstance()->clean();
}
