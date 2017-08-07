#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "data_store.h" 

#define BUFFER_SIZE_EVH 100 

// helper functions
// a function to convert a number to string
char *to_string(long long num) {
    char *result  = (char *) malloc(BUFFER_SIZE_EVH);
    sprintf(result, "%lld", num); 
    return result;
}

// event handlers

void handle_eventhandler(json_t *event) {
    // never free json_t* we get from json_object_get(), since we receive a borrowed reference
    
    // extracting session_id
    json_t *sid =  json_object_get(event, "session_id");
    long long session_id = (long long) json_number_value(sid);
    printf("session id : %lld\n", session_id);
    
    // extracting handle_id
    json_t *hid = json_object_get(event, "handle_id");
    long long handle_id = (long long) json_number_value(hid);
    printf("handle id : %lld\n", handle_id);
    
    //extracting event details from 'event'
    json_t *event_key = json_object_get(event, "event");

    // extracting event name ("attached" or "detached")    
    json_t *evn = json_object_get(event_key, "name");
    const char *event_name = json_string_value(evn);
    printf("event name: %s\n", event_name);

    // extracting opaque_id
    json_t *jstr_oid = json_object_get(event_key, "opaque_id");
    const char *str_oid = json_string_value(jstr_oid);
    printf("opaque_id string: %s\n", str_oid);
    json_error_t err; 
    json_t *opaque_id = json_loads(str_oid, 0, &err);

    // from opaque_id we get -
    
    // user_id
    json_t *uid = json_object_get(opaque_id, "userID");
    char *user_id = (char *) json_string_value(uid); 
    printf("user id : %s\n", user_id);

    // conf_id
    json_t *cid = json_object_get(opaque_id, "confID");
    char *conf_id = (char *) json_string_value(cid);
    printf("conf id : %s\n", conf_id);

    // conf_num
    json_t *cnum = json_object_get(opaque_id, "confNum");
    long long conf_num = (long long)json_integer_value(cnum);
    printf("conf num : %lld\n", conf_num);
   
    // device_id
    json_t *did = json_object_get(opaque_id, "deviceID");
    char *device_id = (char *) json_string_value(did);
    printf("device id: %s\n", device_id);
    
    // setting up data for storing into data store
    user_info user;      
    initialize_user_info(&user);
    user.user_id = user_id;
    user.conf_id = conf_id;
    user.conf_num = to_string(conf_num);
    user.device_id = device_id;
    user.session_id = to_string(session_id);
    user.handle_id = to_string(handle_id);
    
    // storing user info in data store
    initialize_db();
    insert_userinfo(&user);  
    close_db();
    
    // freeing up data after successfully storing data
    free(user.conf_num);
    free(user.session_id);
    free(user.handle_id);
    initialize_user_info(&user);
    json_decref(opaque_id);
}

 
