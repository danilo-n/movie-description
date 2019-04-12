#include "movie.hpp"

/**
 * Callback functions necessary to CURL CURLOPT_WRITEFUNCTION save data
 * from HTTP request.
 *
 * @Return data size on size_t.
 */
size_t cbWriteData( void* readData, size_t size, size_t nmemb, void* userdata)
{
   const size_t realSize = size * nmemb;
   ((string*)userdata)->append((char*)readData, realSize);
   return realSize;
}

/**
 * Prints usage and help message.
 * @Param text indicates program's name.
 */
static void usageMessage(string text)
{
   cout << "Usage: " << text << " <option(s)> ..." << endl
   << setw(15) << left << "-h, --help" << "Displays this information." << endl
   << setw(15) << left << "-a, --apiKey" << "Insert personal OMDB API Key." << endl
   << setw(15) << left << "-s, --search" << "Find a movie information by name." << endl
   << setw(15) << left << "-t, --type" << "Find media type from name" << endl;
}

bool checkParam(string &paramName, string &paramApiKey)
{
   if (paramName.empty()) {
      cout << "invalid production name." << endl;
      return false;
   }
   if (paramApiKey.empty()) {
      cout << "invalid OMDB API Key." << endl;
      return false;
   }
   return true;
}

int curlExec(const string &url, string &data)
{
   int httpCode = 0;
   CURL* curl = curl_easy_init();
   curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cbWriteData);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
   curl_easy_perform(curl);
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
   curl_easy_cleanup(curl);

   return httpCode;
}

string replaceWhiteSpaces(string &origStr)
{
   string retStr(origStr);
   size_t posFoundSpace = origStr.find(" ");

   while (posFoundSpace != string::npos)
   {
      if (posFoundSpace != string::npos){
         retStr.replace(posFoundSpace,1,"");
         retStr.insert(posFoundSpace,"%20");
      }
      posFoundSpace = retStr.find(" ");
   }
   return retStr;
}

string getOmdbUrl(string &paramName, string &paramApiKey)
{
   string strOmdbUrl;
   strOmdbUrl = "http://www.omdbapi.com/?apikey=" + paramApiKey
                + "&t=" + replaceWhiteSpaces(paramName) + "&r=json";
   return strOmdbUrl;
}

int main(int argc, char* argv[])
{
   // Avoid initialize if parameters are invalid.
   if (argc < 2 || argc > 6) {
      usageMessage(argv[0]);
      return 0;
   }

   // Local variables.
   string userApiKey;
   string nameProduction;
   bool getType = false;
   string jsonBuffer;
   int httpCode = 0;

   // Iterate over all parameters.
   for (int i=1; i < argc; i++) {
      string currArg = argv[i];
      if ((currArg.compare("-h") == 0) || (currArg.compare("--help") == 0)) {
         usageMessage(argv[0]);
         return 0;
      } else if ((currArg.compare("-a") == 0) || (currArg.compare("--apiKey") == 0)) {
         userApiKey = argv[++i];
      } else if ((currArg.compare("-s") == 0) || (currArg.compare("--search") == 0)) {
         nameProduction = argv[++i];
      } else if ((currArg.compare("-t") == 0) || (currArg.compare("--type") == 0)) {
         getType = true;
         i++;
      } else {
         usageMessage(argv[0]);
         return 0;
      }
   }

   // Check is every parameter is initialized correctly.
   if (!checkParam(nameProduction, userApiKey)) {
      return -1;
   }

   // Start a HTTP request to OMDB and checks connection result.
   int ret = curlExec(getOmdbUrl(nameProduction, userApiKey), jsonBuffer);
   if (ret == 401) {
      cout << "Unauthorized to access OMDB website." << endl;
      return -1;
   } else if (ret != 200) {
      cout << "Failed to access OMDB website." << endl;
      return -1;
   }

   // Start JSON parsing.
   Json::Reader reader;
   Json::Value dataOmdb;
   reader.parse(jsonBuffer, dataOmdb);

   // If user sets an invalid movie or production name,
   // OMDB will returns an error message on JSON file.
   if (dataOmdb.isMember("Error")) {
      cout << "Could not find any data from movie: " << nameProduction << endl;
      cout << "Error: " << dataOmdb["Error"].asString() << endl;
      return -1;
   }

   // Checks if -t --type option are set.
   if (getType) {
      cout << "Type - " << dataOmdb["Type"].asString() << endl;
   } else {
      // Print on screen JSON data.
      vector<string> memberNames = dataOmdb.getMemberNames();
      const Json::Value& ratingData = dataOmdb["Ratings"];
      for (int index = 0; index < memberNames.size(); index++ ){
         // Ratings is a array, first need to get its index to get its data.
         if (memberNames[index].compare("Ratings") == 0 ) {
            cout << "Ratings -" << endl;
            for (int ratingDataIdx = 0; ratingDataIdx < ratingData.size(); ratingDataIdx++) {
               // Here on ratings are printed only the values, not the key information.
               cout << ratingData[ratingDataIdx]["Source"].asString() << " - "
                    << ratingData[ratingDataIdx]["Value"].asString() << endl;
            }
         } else {
            // Print general data.
            cout << memberNames[index] << " - " << dataOmdb[memberNames[index].c_str()].asString() << endl;
         }
      }
   }

   return 0;
}