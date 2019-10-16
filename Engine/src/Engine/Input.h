#pragma once

#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API Input
	{
	public:
		using Position = std::pair<float, float>;

		inline static bool		IsKeyPressed		(int keycode)			{ return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool		IsMouseButtonPressed(int button)			{ return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static Position	GetMousePosition	(void)					{ return s_Instance->GetMousePositionImpl(); }
		inline static float		GetMouseX			(void)					{ return s_Instance->GetMouseXImpl(); }
		inline static float		GetMouseY			(void)					{ return s_Instance->GetMouseYImpl(); }
		inline static void		Init				   (void)					{ return s_Instance->InitImpl(); }
		inline static Position	GetScroll			(void)					{ return s_Instance->GetScrollImpl(); }
		inline static void		OnUpdate			   (void)					{ return s_Instance->OnUpdateImpl(); }
		inline static Input*	   GetInstance			(void)					{ return s_Instance; }

	protected:
		virtual bool		IsKeyPressedImpl		(int keycode) = 0;
		virtual bool		IsMouseButtonPressedImpl(int button) = 0;
		virtual Position	GetMousePositionImpl	(void) = 0;
		virtual float		GetMouseXImpl			(void) = 0;
		virtual float		GetMouseYImpl			(void) = 0;
		virtual Position	GetScrollImpl			(void) = 0;
		virtual void		OnUpdateImpl			(void) = 0;
		virtual void		InitImpl				   (void) = 0;

		static Input* s_Instance;
	};

}