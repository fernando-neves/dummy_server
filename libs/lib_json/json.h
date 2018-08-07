#ifndef JSON_JSON_H_INCLUDED
# define JSON_JSON_H_INCLUDED

/* C++ INCLUDES */
#include <fstream>

/* JSON LIB INCLUDES */
# include "autolink.h"
# include "value.h"
# include "reader.h"
# include "writer.h"
# include "features.h"

namespace Json{
    static Json::Value load_json_file(std::string file) {
    std::ifstream jsonGet;
    Json::Value root;
    Json::Reader reader;

    jsonGet.open(file);
    reader.parse(jsonGet, root);
    jsonGet.close();

    return root;
}

    static bool save_json_file(std::string file, Json::Value json) {
        std::ofstream jsonSet;

        jsonSet.open(file);
        if (!jsonSet.is_open())
            return false;

        jsonSet << json.toStyledString();
        std::string temp = json.toStyledString();
        jsonSet.close();
        return true;
    }
}


#endif // JSON_JSON_H_INCLUDED
