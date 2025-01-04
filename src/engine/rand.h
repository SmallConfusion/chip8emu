#pragma once

struct rand {
	static unsigned int get();
	static int get(int start, int end);

	static double getDouble(double start, double end);
};