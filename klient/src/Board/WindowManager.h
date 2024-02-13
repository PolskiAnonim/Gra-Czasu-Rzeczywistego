#pragma once
#include "raylib/raylib-cpp.hpp"

class WindowManager {
	inline static raylib::Font font;
	inline static raylib::Vector2 window = raylib::Vector2(800, 800);
    inline static Sound shootSound;

	WindowManager() {
        SetConfigFlags(FLAG_WINDOW_TOPMOST);
		InitWindow(window.x, window.y, "BulletHell.io");
		InitAudioDevice();  
        shootSound = LoadSound("resources/sound.rfx");
		font = LoadFontEx("resources/Monocraft.ttf", 128, nullptr, 0);
		SetTargetFPS(60);
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
	}

	~WindowManager() {
		UnloadFont(WindowManager::font);
		CloseWindow();
	}

public:
	WindowManager(const WindowManager&) = delete;
	WindowManager(WindowManager&&) = delete;
	WindowManager& operator= (const WindowManager&) = delete;
	WindowManager& operator= (WindowManager&&) = delete;

	static WindowManager& getInstance() {
		static WindowManager instance;
		return instance;
	}

	static raylib::Vector2 getWindowSize() {
		return window;
	}

    static raylib::Font& getFont(){
        return font;
    }

    static Sound& getSound(){
        return shootSound;
    }
};