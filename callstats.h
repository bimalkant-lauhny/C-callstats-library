/* \file callstats.h
 * \author Bimalkant Lauhny <lauhny.bimalk@gmail.com>
 * \copyright
 * \brief header file defining functions for sending events to callstast.io REST
 * API.
 */

#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <jwt.h>

#define CALLSTATS_REST_API_VERSION 1.0.0

#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

// helper types and functions for manipulation curl response

struct write_result {
    char *data;
    int pos;
};

typedef struct write_result write_result; 

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream) {
    write_result *result = (struct write_result *)stream;

    if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
    {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;

    return size * nmemb;
}


// authentication function returning access_token

char *authenticate(const char *url, char *jwt, const char *app_id) {
    CURL *curl = NULL;
    CURLcode status;
    struct curl_slist *headers = NULL;
    char *data = NULL;
    long code;
    // val will later store access_token
    char *val = NULL;
    json_error_t err;
    // response will later store server's response
    json_t *response = NULL;
    // access_token will later store 'access_token' field of response object received from server
    json_t *access_token = NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl)
        goto error;

    data = malloc(BUFFER_SIZE);
    if(!data)
        goto error;

    write_result res = {
        .data = data,
        .pos = 0
    };
    
    // set url
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // set HTTP version
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    // append headers
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
     // set headers
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    // formFormat
    char *formFormat= "grant_type=authorization_code&code=%s&client_id=Janus@%s";
    // formDataBuffer 
    char formDataBuffer [BUFFER_SIZE];
    snprintf(formDataBuffer, sizeof(formDataBuffer), formFormat, jwt,  app_id);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formDataBuffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
    // perform request
    status = curl_easy_perform(curl);
    if(status != 0){
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        goto error;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if(code != 200) {
        fprintf(stderr, "error: server responded with code %ld\n", code);
        goto error;
    }

    /* zero-terminate the result */
    data[res.pos] = '\0';

    response = json_loads(data, 0, &err);

    if (response == NULL) {
      perror("JSON parsing error!");
      goto error;
    }

    access_token = json_object_get(response, "access_token");

    if (access_token == NULL) {
      perror("ERROR");
      goto error;
    }

    val = (char*) json_string_value(access_token);
    access_token = NULL;

error:
    if (response) 
        json_decref(response);
    if(data)
        free(data);
    if(curl)
        curl_easy_cleanup(curl);
    if(headers)
        curl_slist_free_all(headers);
    curl_global_cleanup();

    return val;
}
