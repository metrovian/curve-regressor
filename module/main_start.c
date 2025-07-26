#include "sensor_simulator.h"
#include "wrapper_spdlog.h"
#include "wrapper_regressor.h"
#include "server_api.h"
#include "server_database.h"
#include "predefined.h"

pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;

void *main_start(void *argument) {
	server_database_open();
	server_api_open();
	pthread_mutex_lock(&main_mutex);
	while (server_database_status() || server_api_status()) {
		pthread_cond_wait(&main_cond, &main_mutex);
		continue;
	}

	pthread_mutex_unlock(&main_mutex);
	return NULL;
}

void *main_shutdown(void *argument) {
	pthread_mutex_lock(&main_mutex);
	server_api_close();
	server_database_close();
	pthread_cond_signal(&main_cond);
	pthread_mutex_unlock(&main_mutex);
	return NULL;
}

void HANDLE_SIGINT(int32_t signal) {
	pthread_t pthread_shutdown;
	pthread_create(&pthread_shutdown, NULL, main_shutdown, NULL);
	log_warn("SIGINT");
	return;
}

int32_t main(int32_t argc, char *argv[]) {
	signal(SIGINT, HANDLE_SIGINT);
	pthread_t pthread_start;
	pthread_create(&pthread_start, NULL, main_start, NULL);
	pthread_join(pthread_start, NULL);
	return 0;
}