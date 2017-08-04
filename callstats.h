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

