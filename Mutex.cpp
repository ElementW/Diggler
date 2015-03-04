#include "Mutex.hpp"
#include <cstdlib>
#include <pthread.h>

namespace Diggler {

Mutex::Mutex() {
	data = (void*)std::malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t*)data, nullptr);
}

Mutex::~Mutex() {
	pthread_mutex_destroy((pthread_mutex_t*)data);
	free(data);
}

void Mutex::lock() {
	pthread_mutex_lock((pthread_mutex_t*)data);
}

bool Mutex::tryLock() {
	return pthread_mutex_trylock((pthread_mutex_t*)data) == 0;
}

void Mutex::unlock() {
	pthread_mutex_unlock((pthread_mutex_t*)data);
}

}