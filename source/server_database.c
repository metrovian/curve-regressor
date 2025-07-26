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
		log_error("failed to prepare insert query: %s", sqlite3_errmsg(server_database_context));
		return -1;
	}

	char domain_csv[4096] = {0};
	char range_csv[4096] = {0};
	size_t pos_domain = 0;
	size_t pos_range = 0;
	for (int32_t i = 0; i < counts; ++i) {
		pos_domain += snprintf(domain_csv + pos_domain, sizeof(domain_csv) - pos_domain, "%.4G", domain[i]);
		pos_range += snprintf(range_csv + pos_range, sizeof(range_csv) - pos_range, "%.4G", range[i]);
		domain_csv[pos_domain++] = ',';
		domain_csv[pos_domain] = '\0';
		range_csv[pos_range++] = ',';
		range_csv[pos_range] = '\0';
	}

	time_t timenow = time(NULL);
	struct tm *timeinfo = localtime(&timenow);
	char timestamp[64] = {0};
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
	sqlite3_reset(insert_stmt);
	sqlite3_bind_int(insert_stmt, 1, sensor);
	sqlite3_bind_text(insert_stmt, 2, domain_csv, -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 3, range_csv, -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 4, timestamp, -1, SQLITE_STATIC);
	if (sqlite3_step(insert_stmt) != SQLITE_DONE) {
		sqlite3_finalize(insert_stmt);
		log_error("failed to execute insert query");
		return -1;
	}

	sqlite3_finalize(insert_stmt);
	log_info("insert success: %d", counts);
	return 0;
}

extern int8_t server_database_select_sensor_measurements(sensor_simulator_t *sensor, int32_t *counts, int32_t *key, double *domain, double *range, char *datetime) {
	static sqlite3_stmt *select_stmt = NULL;
	static int8_t select_init = 0;
	if (!select_init) {
		const char *select_query =
		    "SELECT sensor, domain_csv, range_csv, id, created_at "
		    "FROM sensor_measurements "
		    "ORDER BY id ASC";

		if (sqlite3_prepare_v2(server_database_context, select_query, -1, &select_stmt, NULL) != SQLITE_OK) {
			log_error("failed to prepare select query: %s", sqlite3_errmsg(server_database_context));
			return -1;
		}
	}

	if (sqlite3_step(select_stmt) == SQLITE_ROW) {
		const char *domain_csv = (const char *)sqlite3_column_text(select_stmt, 1);
		const char *range_csv = (const char *)sqlite3_column_text(select_stmt, 2);
		const char *ptr = domain_csv;
		int32_t iter = 0;
		while (*ptr) {
			domain[iter++] = strtod(ptr, (char **)&ptr);
			while (*ptr == ',') {
				ptr++;
			}
		}

		ptr = range_csv;
		iter = 0;
		while (*ptr) {
			range[iter++] = strtod(ptr, (char **)&ptr);
			while (*ptr == ',') {
				ptr++;
			}
		}

		*counts = iter;
		*sensor = sqlite3_column_int(select_stmt, 0);
		*key = sqlite3_column_int(select_stmt, 3);
		strcpy(datetime, (const char *)sqlite3_column_text(select_stmt, 4));
		log_debug("select success: %d", ++select_init);
		return select_init;
	} else {
		sqlite3_finalize(select_stmt);
		select_stmt = NULL;
		select_init = 0;
		return select_init;
	}
}

extern int8_t server_database_select_sensor_measurement(sensor_simulator_t *sensor, int32_t *counts, int32_t *key, double *domain, double *range) {
	static const char *select_query =
	    "SELECT sensor, domain_csv, range_csv "
	    "FROM sensor_measurements "
	    "WHERE id = ?";

	sqlite3_stmt *select_stmt = NULL;
	if (sqlite3_prepare_v2(server_database_context, select_query, -1, &select_stmt, NULL) != SQLITE_OK) {
		log_error("failed to prepare select query by id: %s", sqlite3_errmsg(server_database_context));
		return -1;
	}

	sqlite3_bind_int(select_stmt, 1, *key);
	if (sqlite3_step(select_stmt) == SQLITE_ROW) {
		const char *domain_csv = (const char *)sqlite3_column_text(select_stmt, 1);
		const char *range_csv = (const char *)sqlite3_column_text(select_stmt, 2);
		const char *ptr = NULL;
		int32_t iter = 0;
		ptr = domain_csv;
		while (*ptr) {
			domain[iter++] = strtod(ptr, (char **)&ptr);
			while (*ptr == ',') {
				ptr++;
			}
		}

		ptr = range_csv;
		iter = 0;
		while (*ptr) {
			range[iter++] = strtod(ptr, (char **)&ptr);
			while (*ptr == ',')
				ptr++;
		}

		*counts = iter;
		*sensor = sqlite3_column_int(select_stmt, 0);
		sqlite3_finalize(select_stmt);
		log_info("select success: %d", *key);
		return 0;
	} else {
		sqlite3_finalize(select_stmt);
		log_warn("failed to execute select query: %d", *key);
		return -1;
	}
}