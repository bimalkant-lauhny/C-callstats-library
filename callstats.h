/* \file callstats.h
 * \author Bimalkant Lauhny <lauhny.bimalk@gmail.com>
 * \copyright
 * \brief header file defining functions for sending events to callstast.io REST
 * API.
 */

#include <string.h>
#include <memory.h>
#include <curl/curl.h>
#include <jansson.h>
#include <jwt.h>

#define CALLSTATS_REST_API_VERSION 1.0.0

char *load_file(char const* path) {
    long length;
    FILE * fp = fopen (path, "rb"); //was "rb"
    // failed opening file
    if (fp == NULL) {
        perror("ERROR! failed opening private key file.");
        return NULL;
    }

    fseek (fp, 0, SEEK_END);
    length = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    char *buffer = (char*)malloc ((length+1)*sizeof(char));
    if (buffer) {
        fread (buffer, sizeof(char), length, fp);
    }
    
    // failed closing file
    int res = fclose (fp);
    if (res != 0) {
        perror("ERROR! failed closing private key file.");
        return NULL;
    }

    buffer[length] = '\0';

    // free the received string yourself
    return buffer;
}

char *get_jwt(char *private_key, char const *keyID, char const *appID) {

    jwt_t *jwt = NULL;

    if (jwt_new(&jwt) < 0) {
        perror("ERROR creating new JWT!");
        return NULL;
    }    

    json_t *json = json_object();
    json_object_set(json, "userID", json_string("Janus"));
    json_object_set(json, "keyID", json_string(keyID));
    json_object_set(json, "appID", json_string(appID));
    char *json_stringified = json_dumps(json, JSON_INDENT(3));

    if(jwt_add_grants_json(jwt, json_stringified) != 0) {
        perror("ERROR adding grants!");
        return NULL;
    }
    
    if (jwt_set_alg(jwt, JWT_ALG_ES256, private_key, strlen(private_key))!= 0) {
        perror("ERROR! could not set algo."); 
        return NULL;
    }

    char *jwt_string = jwt_encode_str(jwt);
    if (jwt_string == NULL) {
        perror("ERROR encoding JWT!");
        return NULL;
    }     

    free(json_stringified);
    jwt_free(jwt);
    
    // free the received string yourself
    return jwt_string;
}
