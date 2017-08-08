// this file has mock tests for testing event_handlers.HACK
#include <stdio.h>
#include "event_handlers.h"

json_t* get_handle_event() {
    json_t *handle = json_object();
    json_object_set_new(handle, "type", json_integer(2));
    json_object_set_new(handle, "timestamp", json_integer(1500625586169884));
    json_object_set_new(handle, "session_id", json_integer(96435475150506));
    json_object_set_new(handle, "handle_id", json_integer(4620600987866721));

    json_t *event = json_object();
    json_object_set_new(event, "name", json_string("attached"));
    json_object_set_new(event, "plugin", json_string("janus.plugin.videoroom"));
    
    json_t *opaque_id = json_object();
    json_object_set_new(opaque_id, "userID", json_string("bimal"));
    json_object_set_new(opaque_id, "deviceID", json_string("P6eDzoDArQki"));
    json_object_set_new(opaque_id, "confID", json_string("Demo Room"));
    json_object_set_new(opaque_id, "confNum", json_integer(1234));
    
    char *temp = json_dumps(opaque_id, 0);
    json_object_set_new(event, "opaque_id", json_string(temp));

    json_object_set_new(handle, "event", event);
    
    free(temp);
    temp = json_dumps(handle, JSON_INDENT(3));
    printf("Handle Event Created: %s\n", temp);
    
    free(temp);
    json_decref(opaque_id);
    return handle;
}

int main(void) {
    
    json_t *handle = get_handle_event();
    handle_eventhandler(handle);
    json_decref(handle);

    return 0;
}
