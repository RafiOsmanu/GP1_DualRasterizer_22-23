#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <iostream>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	class Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio) :
			origin{ _origin },
			fovAngle{ _fovAngle },
			aspectRatio{_aspectRatio}
		{

		}

		void calculateViewMatrix()
		{
			//assign basic view matrix
			viewMatrix = invViewMatrix = Matrix::CreateLookAtLH(origin, forward, up);

			//inverse the invView matrix variable
			invViewMatrix = Matrix::Inverse(invViewMatrix);
		}

		void calculateviewProjectionMatrix()
		{
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
			const float nearPlane{ 0.1f };
			const float farPlane{ 100.f };

			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

			viewProjectionMatrix = viewMatrix * projectionMatrix;
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const float CameraMovementSpeed{ 10.f };

			//Camera Update Logic
			//...
			Matrix finalRotation{ Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw) };

			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			right = finalRotation.TransformVector(Vector3::UnitX);
			right.Normalize();

			up = finalRotation.TransformVector(Vector3::UnitY);
			up.Normalize();

			// new input
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			origin += (pKeyboardState[SDL_SCANCODE_W] + -pKeyboardState[SDL_SCANCODE_S]) * CameraMovementSpeed * deltaTime * forward;
			origin += (-pKeyboardState[SDL_SCANCODE_A] + pKeyboardState[SDL_SCANCODE_D]) * CameraMovementSpeed * deltaTime * right;


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			float slowCSpeed{ CameraMovementSpeed / 2.f };

			if (mouseState & SDL_BUTTON_LMASK)
			{
				if (mouseState & SDL_BUTTON_RMASK)
				{
					origin += mouseY * slowCSpeed * 4.f * up * deltaTime;
					origin -= mouseX * slowCSpeed * 4.f * right * deltaTime;
				}
				else
				{
					origin += mouseY * slowCSpeed * 4.f * forward * deltaTime;
					//totalYaw += mouseX * slowCSpeed * deltaTime;
				}
			}
			else if (mouseState & SDL_BUTTON_RMASK)
			{
				totalPitch -= mouseY * slowCSpeed * deltaTime;
				totalYaw += mouseX * slowCSpeed * deltaTime;
			}
			//assert(false && "Not Implemented Yet");



			//Update Matrices
			calculateViewMatrix();
			calculateviewProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}

		Matrix GetViewMatrix()
		{
			return viewMatrix;
		}

		Matrix GetInvViewMatrix()
		{
			return invViewMatrix;
		}
		Matrix GetProjectionMatrix()
		{
			return projectionMatrix;
		}

		Matrix GetViewProjectionMatrix()
		{
			return viewProjectionMatrix;
		}
		Vector3 GetOrigin()
		{
			return origin;
		}

	private:
		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix worldMatrix{};
		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};
		Matrix viewProjectionMatrix{};
		float aspectRatio{ 1.f };

	};
}

