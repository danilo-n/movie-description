#include <iostream>
#include <string>
#include <iomanip>
#include <jsoncpp/json/json.h>
#include <curl/curl.h>

using namespace std;

/**
 * Check if production name and OMDB API key are set.
 * @param paramName is the production name string.
 * @param paramApiKey is the API Key string.
 * @Return true if everything is ok, or false is case
 * of some parameter is missing.
 */
bool checkParam(string &paramName, string &paramApiKey);

/**
 * Start and execute Curl functions to get JSON file
 * from OMDB webservice.
 *
 * Curl do not allow whitespaces on URL address.
 *
 * @param url a string with URL address to request JSON data.
 * @param data returns JSON data on a string format.
 * @Return a integer indication the HTTP response code.
 */
int curlExec(const string &url, string &data);

/**
 * As Curl do not allow whitespaces on URL address, this
 * function replaces whitespaces with "%20".
 * @param origStr receives the string to remove whitespaces
 * @Return A string without any whitespaces.
 */
string replaceWhiteSpaces(string &origStr);

/**
 * Create a URL address to access the production data on
 * OMDB database.
 * @param paramName is the production name string.
 * @param paramApiKey is the API Key string.
 * @Return A string with the full URL address to the OMDB database.
 */
string getOmdbUrl(string &paramName, string &paramApiKey);