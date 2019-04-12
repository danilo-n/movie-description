# movie-description

To compile:
g++ -o movie movie.cpp -ljsoncpp -lcurl

* It depends on libcurl and jsoncpp.

* Needs a valid API Key to execute requests.

* Example:
$ ./movie --help
Usage: ./movie <option(s)> ...
-h, --help     Displays this information.
-a, --apiKey   Insert personal OMDB API Key.
-s, --search   Find a movie information by name.
-t, --type     Find media type from name

