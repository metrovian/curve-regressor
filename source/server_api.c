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

typedef struct {
	sensor_simulator_t sensor;
	int32_t counts;
	int32_t key;
	double domain[SENSOR_DATA_COUNTS];
	double range[SENSOR_DATA_COUNTS];
	char datetime[64];
} server_api_json_t;

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

static int32_t server_api_sensor_selector_handler(struct mg_connection *connection, void *sensor) {
	char response[4096] = {0};
	server_api_json_t json = {0};
	int32_t ptr = snprintf(response + ptr, sizeof(response) - ptr, "[");
	for (int32_t i = 0; i < SENSOR_DATA_MAX; ++i) {
		if (server_database_select_sensor_measurements(&json.sensor, &json.counts, &json.key, json.domain, json.range, json.datetime) == 0) {
			snprintf(response + ptr - 1, sizeof(response) - ptr, "]");
			mg_printf(
			    connection,
			    "HTTP/1.1 200 OK\r\n"
			    "Content-Type: text/plain\r\n"
			    "Cache-Control: no-cache\r\n"
			    "\r\n"
			    "%s\n",
			    response);

			log_info("select success: %d", RESPONSE_OK);
			return RESPONSE_OK;
		} else {
			ptr += snprintf(
			    response + ptr,
			    sizeof(response) - ptr,
			    "{ "
			    "\"id\": %d, "
			    "\"created_at\": \"%s\" "
			    "},",
			    json.key,
			    json.datetime);
		}
	}

	log_error("load api: %d", RESPONSE_FORBIDDEN);
	return server_api_response(connection, RESPONSE_FORBIDDEN);
}

static int32_t server_api_sensor_calibrator_handler(struct mg_connection *connection, void *sensor) {
	char query_string[64] = {0};
	const struct mg_request_info *request_info = mg_get_request_info(connection);
	mg_get_var(
	    request_info->query_string,
	    strlen(request_info->query_string),
	    "id",
	    query_string,
	    sizeof(query_string));

	server_api_json_t json = {.key = atoi(query_string)};
	if (server_database_select_sensor_measurement(&json.sensor, &json.counts, &json.key, json.domain, json.range) < 0) {
		log_error("calibrate api: %d", RESPONSE_BAD_REQUEST);
		return server_api_response(connection, RESPONSE_BAD_REQUEST);
	}

	double parameters[64] = {0};
	double residual = 0;
	double argument_double = 0;
	int8_t argument_byte = 0;
	switch (json.sensor) {
	case SENSOR_RTD:
		argument_byte = 2;
		residual = calibrate_classic(MODEL_POLYNOMIAL, json.counts, json.domain, json.range, &argument_byte, parameters);
		mg_printf(
		    connection,
		    "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/plain\r\n"
		    "Cache-Control: no-cache\r\n"
		    "\r\n"
		    "R=%+.4ET^2%+.4ET%+4E\n",
		    parameters[0],
		    parameters[1],
		    parameters[2]);

		log_debug("rtd calibration: %.4E", residual);
		break;

	case SENSOR_PD:
		argument_byte = 2;
		residual = calibrate_classic(MODEL_RECIPROCAL, json.counts, json.domain, json.range, &argument_byte, parameters);
		mg_printf(
		    connection,
		    "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/plain\r\n"
		    "Cache-Control: no-cache\r\n"
		    "\r\n"
		    "V=%+.4E/R^2%+.4E\n",
		    parameters[0],
		    parameters[1]);

		log_debug("pd calibration: %+.4E", residual);
		break;

	case SENSOR_LVDT:
		argument_byte = 3;
		residual = calibrate_classic(MODEL_POLYNOMIAL, json.counts, json.domain, json.range, &argument_byte, parameters);
		mg_printf(
		    connection,
		    "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/plain\r\n"
		    "Cache-Control: no-cache\r\n"
		    "\r\n"
		    "V=%+.4ED^3%+.4ED^2%+.4ED%+.4E\n",
		    parameters[0],
		    parameters[1],
		    parameters[2],
		    parameters[3]);

		log_debug("lvdt calibration: %+.4E", residual);
		break;

	case SENSOR_MMI:
		argument_double = 1.5500E-03;
		residual = calibrate_classic(MODEL_SINUSOIDAL, json.counts, json.domain, json.range, &argument_double, parameters);
		mg_printf(
		    connection,
		    "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/plain\r\n"
		    "Cache-Control: no-cache\r\n"
		    "\r\n"
		    "V=%+.4Esin(kD)%+.4Ecos(kD)\n",
		    parameters[0],
		    parameters[1]);

		log_debug("mmi calibration: %+.4E", residual);
		break;

	case SENSOR_MZI:
		argument_double = 1.5500E-01;
		residual = calibrate_classic(MODEL_SINUSOIDAL, json.counts, json.domain, json.range, &argument_double, parameters);
		mg_printf(
		    connection,
		    "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/plain\r\n"
		    "Cache-Control: no-cache\r\n"
		    "\r\n"
		    "V=%+.4Esin(kN)%+.4Ecos(kN)\n",
		    parameters[0],
		    parameters[1]);

		log_debug("mzi calibration: %+.4E", residual);
		break;
	}

	log_info("calibrate success: %d", RESPONSE_OK);
	return RESPONSE_OK;
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
	mg_set_request_handler(server_api_context, "/api/sensor/data", server_api_sensor_selector_handler, NULL);
	mg_set_request_handler(server_api_context, "/api/sensor/calibrate", server_api_sensor_calibrator_handler, NULL);
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