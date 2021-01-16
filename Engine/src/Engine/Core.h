#pragma once

#include <memory>
#include <string>

#ifdef PLATFORM_WINDOWS
#if DYNAMIC_LINK
#ifdef BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif
#else
#error Engine only supports Windows!
#endif

#ifdef RELEASE
#define ENABLE_ASSERTS
#endif

#ifdef DEBUG
#define ENABLE_ASSERTS
#endif

#pragma warning (disable : 4353)

#ifdef ENABLE_ASSERTS
#define ASSERT(x, ...) { if(!(x)) { ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define ASSERT_FAIL(...) ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();
#define CORE_ASSERT(x, ...) { if(!(x)) { ERROR_CORE("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define ASSERT(x, ...)
#define ASSERT_FAIL(...)
#define CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

using uint = uint32_t;
using uint64 = uint64_t;
using int64 = int64_t;


namespace Engine {
	template<typename T>
	using UPtr = std::unique_ptr<T>;

	template<typename T>
	using SPtr = std::shared_ptr<T>;

	template<typename T>
	using WPtr = std::weak_ptr<T>;

	template <class T, class... Args>
	SPtr<T> MakeShared(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}


const std::string projectDir = "D:/Projects/Git/Engine/";