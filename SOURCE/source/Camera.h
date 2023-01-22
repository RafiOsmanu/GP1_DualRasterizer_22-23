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
			m_Origin{ _origin },
			m_FovAngle{ _fovAngle },
			m_AspectRatio{_aspectRatio}
		{

		}

		void calculateViewMatrix()
		{
			//assign basic view matrix
			m_ViewMatrix = m_InvViewMatrix = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up);

			//inverse the invView matrix variable
			m_InvViewMatrix = Matrix::Inverse(m_InvViewMatrix);
		}

		void calculateviewProjectionMatrix()
		{
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
			const float nearPlane{ 0.1f };
			const float farPlane{ 100.f };

			m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_Fov, m_AspectRatio, nearPlane, farPlane);

			m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
		}

		void Update(const Timer* pTimer)
		{
			const float& deltaTime = pTimer->GetElapsed();
			const float CameraMovementSpeed{ 10.f };

			//Camera Update Logic
			//...
			Matrix finalRotation{ Matrix::CreateRotationX(m_TotalPitch) * Matrix::CreateRotationY(m_TotalYaw) };

			m_Forward = finalRotation.TransformVector(Vector3::UnitZ);
			m_Forward.Normalize();

			m_Right = finalRotation.TransformVector(Vector3::UnitX);
			m_Right.Normalize();

			m_Up = finalRotation.TransformVector(Vector3::UnitY);
			m_Up.Normalize();

			// new input
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			m_Origin += (pKeyboardState[SDL_SCANCODE_W] + -pKeyboardState[SDL_SCANCODE_S]) * CameraMovementSpeed * deltaTime * m_Forward;
			m_Origin += (-pKeyboardState[SDL_SCANCODE_A] + pKeyboardState[SDL_SCANCODE_D]) * CameraMovementSpeed * deltaTime * m_Right;


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			float slowCSpeed{ CameraMovementSpeed / 2.f };

			if (mouseState & SDL_BUTTON_LMASK)
			{
				if (mouseState & SDL_BUTTON_RMASK)
				{
					m_Origin += mouseY * slowCSpeed * 2.f * m_Up * deltaTime;
					m_Origin -= mouseX * slowCSpeed * 2.f * m_Right * deltaTime;
				}
				else
				{
					m_Origin += mouseY * slowCSpeed * 2.f * m_Forward * deltaTime;
					//totalYaw += mouseX * slowCSpeed * deltaTime;
				}
			}
			else if (mouseState & SDL_BUTTON_RMASK)
			{
				m_TotalPitch -= mouseY * slowCSpeed * deltaTime;
				m_TotalYaw += mouseX * slowCSpeed * deltaTime;
			}
			//assert(false && "Not Implemented Yet");



			//Update Matrices
			calculateViewMatrix();
			calculateviewProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}

		Matrix GetViewMatrix()
		{
			return m_ViewMatrix;
		}

		Matrix GetInvViewMatrix()
		{
			return m_InvViewMatrix;
		}
		Matrix GetProjectionMatrix()
		{
			return m_ProjectionMatrix;
		}

		Matrix GetViewProjectionMatrix()
		{
			return m_ViewProjectionMatrix;
		}
		Vector3 GetOrigin()
		{
			return m_Origin;
		}

	private:
		Vector3 m_Origin{};
		float m_FovAngle{ 90.f };
		float m_Fov{ tanf((m_FovAngle * TO_RADIANS) / 2.f) };

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch{};
		float m_TotalYaw{};

		Matrix m_WorldMatrix{};
		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};
		Matrix m_ViewProjectionMatrix{};
		float m_AspectRatio{ 1.f };

	};
}

