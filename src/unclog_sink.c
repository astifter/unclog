#define _BSD_SOURCE

#include "unclog_int.h"

#include <stdlib.h>
#include <string.h>

unclog_sink_t* unclog_sink_create(unclog_values_t* defaults, const char* sink) {
	unclog_sink_t* handle = malloc(sizeof(unclog_sink_t));
	memset(handle, 0, sizeof(unclog_sink_t));

	memcpy(&handle->common, defaults, sizeof(unclog_values_t));
	handle->sink = strdup(sink);

	return handle;
}

void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value) {
	unclog_keyvalue_t* kv = malloc(sizeof(unclog_keyvalue_t));
	kv->key = strdup(key);
	kv->value = strdup(value);

	kv->next = sink->values;
	sink->values = kv;
}
