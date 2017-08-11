/*! \file    event_handlers.h
 * \author   Bimalkant Lauhny <lauhny.bimalk@gmail.com>
 * \copyright MIT License
 * \brief    Methods for handling events received from Janus 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "data_store.h" 
#include "callstats.h"

#define BUFFER_SIZE_EVH 100 

// helper functions

// a function to convert a number to string
char *to_string(long long num) {
    char *result  = (char *) malloc(BUFFER_SIZE_EVH);
    sprintf(result, "%lld", num); 
    return result;
}

// a function to replace spaces in a string with '-'
char *without_spaces(char *old_str) {
    char *c_old = old_str;
    char *new_str = (char *) malloc(strlen(old_str)+1);
    char *c_new = new_str;
    size_t spaces = 0;
    while(*c_old) {
        if (*c_old == ' ') {
            if (spaces == 0) {
                *c_new = '-';
                spaces++;
                c_new++;
            }
            c_old++;
            continue;
        }
        spaces = 0;
        *c_new = *c_old;
        c_new++;
        c_old++;
    }
    *c_new = '\0';
    return new_str;
}

// event handlers

// event handler for 'session' events (type: 1)
void session_eventhandler(json_t *event) {
    // extracting session_id
    json_t *sid =  json_object_get(event, "session_id");
    long long session_id = (long long) json_number_value(sid);
    printf("session id : %lld\n", session_id);

    //extracting event details from 'event'
    json_t *event_key = json_object_get(event, "event");

    // extracting event name ("attached" or "detached")    
    json_t *evn = json_object_get(event_key, "name");
    const char *event_name = json_string_value(evn);
    printf("event name: %s\n", event_name);
}

// event handler for 'handle' events (type: 2)
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
    if (strcmp(event_name, "attached") == 0) {
        // extracting opaque_id
        json_t *jstr_oid = json_object_get(event_key, "opaque_id");
        if (jstr_oid == NULL) {
            printf("This event does not contain opaque_id. Ignoring!!");
            return;
        }
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
        // def - data_store.h
        user_info user;      
        
        // initializing user
        // def - data_store.h
        initialize_user_info(&user);
        
        // assigning values to user
        user.user_id = user_id;
        // since conf_id is part of post request urls, we will remove spaces from it 
        // def - event_handlers.h
        user.conf_id = without_spaces(conf_id);
        // conf_num, session_id and handle_id are numbers, we need to convert
        // these to strings before storing into database
        // def - event_handlers.h
        user.conf_num = to_string(conf_num);
        user.device_id = device_id;
        user.session_id = to_string(session_id);
        user.handle_id = to_string(handle_id);
        
        // storing user info in data store
        // def - data_store.h
        insert_userinfo(&user); 
        
        // get auth token for the user
        // def - callstats.h
        char *token = callstats_authenticate(user_id); 
        
        // add token to user
        // def - data_store.h
        add_token(user.session_id, user.handle_id, token);
        
        // freeing up data after data is stored successfully
        free(user.conf_id);
        free(user.conf_num);
        free(user.session_id);
        free(user.handle_id);
        free(token);
        printf(":::Here:::\n");
        json_decref(opaque_id);
    } else if(strcmp(event_name, "detached") == 0) {

    }
}

void jsep_eventhandler(json_t *event) {

}

void webrtc_eventhandler(json_t *event) {

}


void media_eventhandler(json_t *event) {

}

// event handler for 'plugin' events (type: 64)
void plugin_eventhandler(json_t *event) {
    // extracting session_id
    json_t *sid =  json_object_get(event, "session_id");
    long long sess_id = (long long) json_number_value(sid);
    printf("session id : %lld\n", sess_id);
    char *session_id = to_string(sess_id);
    
    // extracting handle_id
    json_t *hid = json_object_get(event, "handle_id");
    long long hand_id = (long long) json_number_value(hid);
    printf("handle id : %lld\n", hand_id);
    char *handle_id = to_string(hand_id);
    
    //extracting event details from 'event'
    json_t *event_key = json_object_get(event, "event");

    // extracting 'data' from 'event'
    json_t *data= json_object_get(event_key, "data");

    // extracting event name from 'data'
    json_t *evn = json_object_get(data, "event");
    const char *event_name = json_string_value(evn);
    printf("event name: %s\n", event_name);
    
    // if not 'joined' event, leave
    if (strcmp(event_name, "joined") == 0) {
        // extracting 'user_num'
        json_t *unum = json_object_get(data, "id");
        long long usr_num = (long long)json_integer_value(unum);
        printf("user num : %lld\n", usr_num);
        char *user_num = to_string(usr_num);

        // storing user info in data store
        size_t rc = add_user_num(session_id, handle_id, user_num);
        free(user_num);
        user_info user;
        initialize_user_info(&user);
        
        //fetch user info for a combination of session_id and handle_id from data store
        // def - data_store.h
        get_user_info(session_id, handle_id, &user);
            
        // extracting timestamp to send to callstats_user_joined
        json_t *t_stamp = json_object_get(event, "timestamp");
        long long timestamp = (long long) json_number_value(t_stamp);
        
        // send user-join request to callstats REST API
        // def - callstats.h
        char *uc_id  = callstats_user_joined(&user, timestamp);
        free(uc_id);
        // free the memory allocated to user info in 'user'
        // def - data_store.h
        free_user_info(&user);
        
    } else if (strcmp(event_name, "unpublished") == 0) {
        
    }

    free(session_id);
    free(handle_id);
}

void transport_eventhandler(json_t *event) {

}

// event handler for 'core' events (type: 256)
void core_eventhandler(json_t *event) {
    json_t *event_key = json_object_get(event, "event"); 
    json_t *json_status = json_object_get(event_key, "status");
    const char *status = json_string_value(json_status); 
    if (strcmp(status, "started") == 0) {
      // initializing database
      initialize_db();
    } else if (strcmp(status, "shutdown") == 0){
      // closing database
      close_db();
    }
}

