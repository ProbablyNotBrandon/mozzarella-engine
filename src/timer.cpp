#include "timer.h"

std::chrono::steady_clock::time_point start;
int time_limit_ms;


/*
 * Start the timer by setting the global start time variable to the current time instant.
 */
void start_timer(int ms) {
    time_limit_ms = ms;
    start = std::chrono::steady_clock::now();
}


/*
 * Return true if the elapsed time on the timer has exceeded the time limit.
 */
bool time_up() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

    return elapsed >= time_limit_ms;
}
