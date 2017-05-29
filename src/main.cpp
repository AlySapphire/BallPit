#include "BallPitApp.h"

int main() {
	
	auto app = new BallPitApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}