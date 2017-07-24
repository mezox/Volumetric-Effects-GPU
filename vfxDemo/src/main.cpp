#include "SmokeDemo.h"
#include "SponzaDemo.h"

#include <iostream>

int main(int argc, char* argv[])
{
	auto smoke = new vfx::SmokeDemo();
	auto sponza = new vfx::SponzaDemo();

	vfx::engine::Engine::initialize();
	vfx::engine::Engine::addScene("smoke", smoke);
	vfx::engine::Engine::addScene("sponza", sponza);

	if (argc > 1 && std::string(argv[1]) == "sponza")
	{
		vfx::engine::Engine::push("sponza");
	}
	else
		vfx::engine::Engine::push("smoke");
	
	vfx::engine::Engine::run();

	delete smoke;
	delete sponza;
}