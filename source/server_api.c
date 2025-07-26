#include "server_api.h"
#include "wrapper_regressor.h"
#include "wrapper_spdlog.h"
#include "sensor_simulator.h"
#include "server_database.h"
#include "predefined.h"
#include "civetweb.h"

static struct mg_context *server_api_context = NULL;

typedef enum {
	RESPONSE_OK = 200,
	RESPONSE_CREATED = 201,
	RESPONSE_NO_CONTENT = 204,
	RESPONSE_BAD_REQUEST = 400,
	RESPONSE_UNAUTHORIZED = 401,
	RESPONSE_FORBIDDEN = 403,
	RESPONSE_NOT_FOUND = 404,
	RESPONSE_CONFLICT = 409,
	RESPONSE_INTERNAL_SERVER_ERROR = 500,
	RESPONSE_NOT_IMPLEMENTED = 501,
	RESPONSE_SERVICE_UNAVAILABLE = 503,
} server_api_response_t;

static int32_t server_api_response(struct mg_connection *connection, server_api_response_t response) {
	// clang-format off
	const char *status_message = NULL;
	switch (response) {
        case RESPONSE_OK: status_message = "OK"; break;
        case RESPONSE_CREATED: status_message = "Created"; break;
        case RESPONSE_NO_CONTENT: status_message = "No Content"; break;
        case RESPONSE_BAD_REQUEST: status_message = "Bad Request"; break;
        case RESPONSE_UNAUTHORIZED: status_message = "Unauthorized"; break;
        case RESPONSE_FORBIDDEN: status_message = "Forbidden"; break;
        case RESPONSE_NOT_FOUND: status_message = "Not Found"; break;
        case RESPONSE_CONFLICT: status_message = "Conflict"; break;
        case RESPONSE_INTERNAL_SERVER_ERROR: status_message = "Internal Server Error"; break;
        case RESPONSE_NOT_IMPLEMENTED: status_message = "Not Implemented"; break;
        case RESPONSE_SERVICE_UNAVAILABLE: status_message = "Service Unavailable"; break;
        default: status_message = "Unknown"; break;
	}
	// clang-format on
	mg_printf(
	    connection,
	    "HTTP/1.1 %d %s\r\n"
	    "Content-Type: text/plain\r\n"
	    "Cache-Control: no-cache\r\n"
	    "\r\n"
	    "%d %s\n",
	    response, status_message,
	    response, status_message);

	return response;
}

static int32_t server_api_sensor_simulator_handler(struct mg_connection *connection, void *sensor) {
	double domain[SENSOR_DATA_COUNTS] = {0};
	double range[SENSOR_DATA_COUNTS] = {0};
	if (sensor_simulator((sensor_simulator_t)sensor, SENSOR_DATA_COUNTS, domain, range) < 0) {
		log_critical("acquire api: %d", RESPONSE_NOT_IMPLEMENTED);
		return server_api_response(connection, RESPONSE_NOT_IMPLEMENTED);
	}

	server_database_insert_sensor_measurements((sensor_simulator_t)sensor, SENSOR_DATA_COUNTS, domain, range);
	log_info("acquire success: %d", RESPONSE_OK);
	return server_api_response(connection, RESPONSE_OK);
}

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

	mg_set_request_handler(server_api_context, "/api/sensor/rtd", server_api_sensor_simulator_handler, (void *)SENSOR_RTD);
	mg_set_request_handler(server_api_context, "/api/sensor/pd", server_api_sensor_simulator_handler, (void *)SENSOR_PD);
	mg_set_request_handler(server_api_context, "/api/sensor/lvdt", server_api_sensor_simulator_handler, (void *)SENSOR_LVDT);
	mg_set_request_handler(server_api_context, "/api/sensor/mmi", server_api_sensor_simulator_handler, (void *)SENSOR_MMI);
	mg_set_request_handler(server_api_context, "/api/sensor/mzi", server_api_sensor_simulator_handler, (void *)SENSOR_MZI);
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