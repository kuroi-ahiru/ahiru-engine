#pragma once

#include <memory>
#include <string>
#include <streambuf>
#include <vector>
#include <iostream>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include "GameObject.h"
#include "Scene.h"


class IEventProcessor {
public:
	virtual void processEvent(const SDL_Event& event) = 0;
};

class ConsoleBuffer : public std::streambuf {
public:
	ConsoleBuffer(std::vector<std::string>& log) : log(log) {}

protected:
	virtual int overflow(int c) override {
		if (c == '\n') {
			log.push_back(current_line);
			current_line.clear();
		}
		else {
			current_line += static_cast<char>(c);
		}
		return c;
	}

private:
	std::vector<std::string>& log;
	std::string current_line;
};

class MyWindow {

	SDL_Window* _window = nullptr;
	void* _ctx = nullptr;

	unsigned short _width = 800;
	unsigned short _height = 600;

	std::string droppedFile;

public:
	int width() const { return _width; }
	int height() const { return _height; }
	double aspect() const { return static_cast<double>(_width) / _height; }

	auto* windowPtr() const { return _window; }
	auto* contextPtr() const { return _ctx; }

	MyWindow(const char* title, unsigned short width, unsigned short height);
	MyWindow(MyWindow&&) noexcept = delete;
	MyWindow(const MyWindow&) = delete;
	MyWindow& operator=(const MyWindow&) = delete;
	~MyWindow();

	void open(const char* title, unsigned short width, unsigned short height);
	void close();
	bool isOpen() const { return _window; }

	bool processEvents(IEventProcessor* event_processor = nullptr);
	void swapBuffers() const;
	void display_func(std::shared_ptr<GameObject> selectedObject, Scene& scene);
	void logMessage(const std::string& message);
	std::string getDroppedFile();

    void LoadIcons(Scene &scene);
	bool IsPaused() const { return isPaused; }

private:
	std::vector<std::string> console_log;
	ConsoleBuffer console_buffer{ console_log };
	std::streambuf* original_cout_buffer = nullptr;
	bool isPaused = false;
};