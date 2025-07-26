#include "server_database.h"
#include "wrapper_spdlog.h"
#include "predefined.h"
#include "sqlite3.h"

static sqlite3 *server_database_context = NULL;

extern int8_t server_database_open() {
	if (sqlite3_open(NETWORK_SQLITE_DB, &server_database_context) != SQLITE_OK) {
		log_error("failed to start database service: %s", sqlite3_errmsg(server_database_context));
		return -1;
	}

	FILE *fptr = fopen(NETWORK_SQLITE_SCHEMA, "rb");
	if (!fptr) {
		log_error("failed to open schema: %s", NETWORK_SQLITE_SCHEMA);
		return -1;
	}

	if (fseek(fptr, 0, SEEK_END) != 0) {
		fclose(fptr);
		log_error("failed to seek schema");
		return -1;
	}

	long fsize = ftell(fptr);
	char *schema = (char *)malloc(fsize + 1);
	if (!schema) {
		fclose(fptr);
		log_error("failed to allocate schema: %ld", fsize);
		return -1;
	}

	rewind(fptr);
	if (fread(schema, 1, fsize, fptr) != fsize) {
		fclose(fptr);
		log_error("failed to read schema: %ld", fsize);
	}

	fclose(fptr);
	schema[fsize] = 0;
	if (sqlite3_exec(server_database_context, schema, NULL, NULL, NULL) != SQLITE_OK) {
		log_error("failed to execute schema");
		return -1;
	}

	log_info("database service started");
	return 0;
}

extern int8_t server_database_close() {
	if (server_database_context) {
		sqlite3_close(server_database_context);
		server_database_context = NULL;
		log_info("database service terminated");
		return 0;
	}

	return -1;
}

extern int8_t server_database_status() {
	return server_database_context != NULL;
}

extern int8_t server_database_insert_sensor_measurements(sensor_simulator_t sensor, int32_t counts, double *domain, double *range) {
	static const char *insert_query =
	    "INSERT "
	    "INTO sensor_measurements (sensor, domain_csv, range_csv, created_at) "
	    "VALUES (?, ?, ?, ?)";

	sqlite3_stmt *insert_stmt = NULL;
	if (sqlite3_prepare_v2(server_database_context, insert_query, -1, &insert_stmt, 0) != SQLITE_OK) {
		log_error("failed to prepare insert query");
		return -1;
	}

	time_t timenow = time(NULL);
	struct tm *timeinfo = localtime(&timenow);
	char timestamp[64] = {0};
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
	for (int32_t i = 0; i < counts; ++i) {
		sqlite3_reset(insert_stmt);
		sqlite3_bind_int(insert_stmt, 1, sensor);
		sqlite3_bind_double(insert_stmt, 2, domain[i]);
		sqlite3_bind_double(insert_stmt, 3, range[i]);
		sqlite3_bind_text(insert_stmt, 4, timestamp, -1, SQLITE_STATIC);
		if (sqlite3_step(insert_stmt) != SQLITE_DONE) {
			sqlite3_finalize(insert_stmt);
			log_error("failed to execute insert query: %d", i);
			return -1;
		}
	}

	sqlite3_finalize(insert_stmt);
	log_info("insert success: %d", counts);
	return 0;
}