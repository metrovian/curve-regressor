#include "server_api.h"
#include "wrapper_regressor.h"
#include "wrapper_spdlog.h"
#include "predefined.h"
#include "civetweb.h"

static struct mg_context *server_api_context = NULL;

extern int8_t server_api_open() {
	const char *server_options[] = {
	    "document_root", NETWORK_CIVETWEB_ROOT,
	    "listening_ports", NETWORK_CIVETWEB_PORT, NULL};

	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	server_api_context = mg_start(&callbacks, NULL, server_options);
	if (!server_api_context) {
		log_error("failed to start api service");
		return -1;
	}

	log_info("api service started");
	return 0;
}

extern int8_t server_api_close() {
	if (server_api_context) {
		mg_stop(server_api_context);
		server_api_context = NULL;
		log_info("api service terminated");
	}

	return 0;
}

extern int8_t server_api_status() {
	return server_api_context != NULL;
}