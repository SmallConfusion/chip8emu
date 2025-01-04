#include "rand.h"
#include <chrono>
#include <random>

std::mt19937 rng{static_cast<unsigned int>(
	std::chrono::steady_clock::now().time_since_epoch().count())};

unsigned int rand::get() {
	return rng();
}

int rand::get(int start, int end) {
	return std::uniform_int_distribution<int>(start, end)(rng);
}

double rand::getDouble(double start, double end) {
	return std::uniform_real_distribution<double>(start, end)(rng);
}
