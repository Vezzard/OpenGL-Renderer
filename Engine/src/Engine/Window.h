#pragma once

#include "pch.h"

#include "Engine/Core.h"
#include "Engine/Events/Event.h"

namespace Engine {

   struct WindowProps
   {
      std::string Title;
      uint Width;
      uint Height;

      WindowProps(const std::string& title = "Engine",
         uint width = 1280,
         uint height = 720)
         : Title(title), Width(width), Height(height)
      {
      }
   };

   // Interface representing a desktop system based Window
   class ENGINE_API Window
   {
   public:
      using EventCallbackFn = std::function<void(Event&)>;

      virtual ~Window() = default;

      virtual void OnUpdate() = 0;

      virtual uint GetWidth() const = 0;
      virtual uint GetHeight() const = 0;

      // Window attributes
      virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
      virtual void SetVSync(bool enabled) = 0;
      virtual bool IsVSync() const = 0;

      virtual void* GetNativeWindow() const = 0;

      static Window* Create(const WindowProps& props = WindowProps());
   };

}