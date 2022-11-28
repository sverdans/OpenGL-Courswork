#pragma once
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Resources/ResourceManager.h"
#include "../../System/InputHandler.h"
#include "../../System/Constants.h"
#include "../../System/Window.h"
#include "../../System/Time.h"
#include "../GameObject.h"

#include "BehaviourComponent.h"
#include "TransformComponent.h"


class CameraComponent : public BehaviourComponent
{
public:
	enum class ProjectionMode
	{
		Perspective,
		Orthographic
	};

private:
	ProjectionMode projectionMode;

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);

	bool isMovable;

	float orthoScale = 1;

	float farClipPlane;
	float nearClipPlane;

	float viewportWidth = Window::size.x;
	float viewportHeight = Window::size.y;
	float fieldOfView;

public:

	CameraComponent(GameObject* gameObject) : BehaviourComponent(gameObject) {}

	void updateViewMatrix()
	{
		auto TC = gameObject->getComponent<TransformComponent>();
		glm::vec3 position = TC->getGlobalPosition();
		viewMatrix = glm::lookAt(position, position + TC->getForward(), constants::worldUp);
	}

	void updateProjectionMatrix()
	{
		if (projectionMode == ProjectionMode::Perspective)
			projectionMatrix = glm::perspective(
				glm::radians(fieldOfView),
				viewportWidth / viewportHeight,
				nearClipPlane,
				farClipPlane);
		else
			projectionMatrix = glm::ortho(
				-orthoScale * viewportWidth / viewportHeight,
				orthoScale * viewportWidth / viewportHeight,
				-orthoScale,
				orthoScale,
				nearClipPlane,
				farClipPlane);
	}

	void init() override 
	{
		updateViewMatrix();
		updateProjectionMatrix();
	}

	void update() override
	{
		if (!isMovable)
			return;

		viewportWidth = Window::size.x;
		viewportHeight = Window::size.y;

		auto TC = gameObject->getComponent<TransformComponent>();
		auto& keys = InputHandler::getKeys();
		
		float delta = 0.002f * Time::getDeltaTime();
		float deltaR = 0.05f * Time::getDeltaTime();

		glm::vec3 movementDelta(0.f, 0.f, 0.f);
		glm::vec3 rotationDelta(0.f, 0.f, 0.f);

		if (keys[GLFW_KEY_W]) movementDelta.z += delta;
		if (keys[GLFW_KEY_S]) movementDelta.z -= delta;
		
		if (keys[GLFW_KEY_A]) movementDelta.x += delta;	// -= ?
		if (keys[GLFW_KEY_D]) movementDelta.x -= delta; // += ?
		
		if (keys[GLFW_KEY_E]) movementDelta.y += delta;
		if (keys[GLFW_KEY_Q]) movementDelta.y -= delta;

		if (keys[GLFW_KEY_RIGHT]) rotationDelta.y -= deltaR;
		if (keys[GLFW_KEY_LEFT]) rotationDelta.y += deltaR;

		if (keys[GLFW_KEY_UP]) rotationDelta.x -= deltaR;
		if (keys[GLFW_KEY_DOWN]) rotationDelta.x += deltaR;

		TC->setRotation(TC->getRotation() + rotationDelta);
		TC->translate(TC->getForward() * movementDelta.z);
		TC->translate(TC->getRight() * movementDelta.x);
		TC->translate(TC->getUp() * movementDelta.y);
	}

	void setProjectionMode(ProjectionMode mode)
	{
		projectionMode = mode;
		updateProjectionMatrix();
	}
	
	void onPreRender() override
	{
		updateViewMatrix();
		updateProjectionMatrix();

		viewProjectionMatrix = projectionMatrix * viewMatrix;
		
		auto defaultShader = ResourceManager::getShader("defaultShader");
		defaultShader->use();
		defaultShader->setMatrix("viewProjectionMat", viewProjectionMatrix);
		defaultShader->unuse();

		auto monochromeShader = ResourceManager::getShader("monochromeShader");
		monochromeShader->use();
		monochromeShader->setMatrix("viewProjectionMat", viewProjectionMatrix);
		monochromeShader->unuse();
	}

	void deserialize(const nlohmann::json& jsonObject) override
	{
		viewportWidth = Window::size.x;
		viewportHeight = Window::size.y;

		projectionMode = static_cast<ProjectionMode>(jsonObject["ProjectionMode"]);

		isMovable = jsonObject["isMovable"];
		fieldOfView = jsonObject["fieldOfView"];
		farClipPlane = jsonObject["farClipPlane"];
		nearClipPlane = jsonObject["nearClipPlane"];

		updateViewMatrix();
		updateProjectionMatrix();
	}

	void serialize(nlohmann::json& jsonObject) override
	{
		nlohmann::json component;

		component["isMovable"] = isMovable;
		component["fieldOfView"] = fieldOfView;
		component["farClipPlane"] = farClipPlane;
		component["nearClipPlane"] = nearClipPlane;
		component["ProjectionMode"] = static_cast<int>(projectionMode);

		jsonObject["CameraComponent"] = component;
	}
};