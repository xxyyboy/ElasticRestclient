/**
 * @file
 * @brief
 * updated opensource by kenyuan 20200618
 */

#ifndef ELASTICSEARCH_H
#define ELASTICSEARCH_H

#include <string>
#include <sstream>
#include <list>
#include <mutex>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>

#include "connection.h"
#include "json.h"

#define EXCEPTION(...) throw Exception(__FILE__, __LINE__, __VA_ARGS__)

#define EXCEPTION(...) throw Exception(__FILE__, __LINE__, __VA_ARGS__)

class Exception : std::exception {
    public:
        template<typename T>
        Exception(const char* fil, int lin, T const& msg);
        virtual ~Exception() throw() {}

        const char* what() const throw() { return _msg.c_str(); }

    private:
        Exception(){}
        std::string _msg;
};

template<typename T>
Exception::Exception(const char* fil, int lin, T const& msg) {
    _msg = msg;
    std::cerr << "Exception in "<< fil << " l. " << lin << " ->\n";
    std::cerr << msg << std::endl;
}

enum Result {
    OK,
    ERROR,
    MORE_DATA
};

/// API class for elastic search server.
/// Node: Instance of elastic search on server represented by url:port
class ElasticSearch {
    public:
        ElasticSearch(const std::string& node, bool readOnly = false);
        ~ElasticSearch();

        ///set basic auth username & password
        void setBasicAuth(const std::string username, const std::string password);
        
        /// Request document number of type T in index I.
        long unsigned int getDocumentCount(const char* index, const char* type);

        /// Request the document by index/type/id.
        bool getDocument(const char* index, const char* type, Json::Object& msg, const char* id=NULL);

        /// Request the document by index/type/ query key:value.
        void getDocument(const std::string& index, const std::string& type, const std::string& key, const std::string& value, Json::Object& msg);

        /// Delete the document by index/type/id.
        bool deleteDocument(const char* index, const char* type, const char* id=NULL);

        /// Delete the document by index/type.
        bool deleteAll(const char* index, const char* type);

        /// Test if document exists
        bool exist(const std::string& index, const std::string& type, const std::string& id="");

        /// Get Id of document
        bool getId(const std::string& index, const std::string& type, const std::string& key, const std::string& value, const std::string& id="");

        /// Index a document.
        bool index(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id="");

        /// Index a document with automatic id creation
        std::string index(const std::string& index, const std::string& type, const Json::Object& jData);

        /// Update a document field.
        bool update(const std::string& index, const std::string& type, const std::string& key, const std::string& value, const std::string& id="");

        /// Update doccument fields.
        bool update(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id="");

        /// Update or insert if the document does not already exists.
        bool upsert(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id="");

        /// Search API of ES. Specify the doc type.
        long search(const std::string& index, const std::string& type, const std::string& query, Json::Object& result);

        // Bulk API
        bool bulk(const char*, Json::Object& jResult);

    public:
        /// Delete given type (and all documents, mappings)
        bool deleteType(const std::string& index, const std::string& type);

    public:
        /// Test if index exists
        bool exist(const std::string& index);

        /// Create index, optionally with data (settings, mappings etc)
        bool createIndex(const std::string& index, const char* data = NULL);
        
        /// Delete given index (and all types, documents, mappings)
        bool deleteIndex(const std::string& index);
        
        /// Refresh the index.
        void refresh(const std::string& index);

    public:
        /// Initialize a scroll search. Use the returned scroll id when calling scrollNext. Size is based on shardSize. Returns false on error
        bool initScroll(std::string& scrollId, const std::string& index, const std::string& type, const std::string& query, int scrollSize = 1000);

        /// Scroll to next matches of an initialized scroll search. scroll_id may be updated. End is reached when resultArray.empty() is true (in which scroll is automatically cleared). Returns false on error.
        bool scrollNext(std::string& scrollId, Json::Array& resultArray);

        /// Clear an initialized scroll search prior to its automatically 1 minute timeout
        void clearScroll(const std::string& scrollId);

        /// Perform a scan to get all results from a query.
        int fullScan(const std::string& index, const std::string& type, const std::string& query, Json::Array& resultArray, int scrollSize = 1000);

        ///
        int httpcode(){ return m_reshttpcode;}
        std::string httpbody(){ return m_reshttpbody;}
    private:
        void appendHitsToArray(const Json::Object& msg, Json::Array& resultArray);

    private:
        /// Private constructor.
        ElasticSearch();
        
        ///
        void clean();

        /// Read Only option, all index functions return false.
        bool _readOnly;
        
        ///
        bool m_auth;
        
        ///
        std::string m_username;
        
        ///
        std::string m_password;
        
        ///
        RestClient::Response m_res;
        
        ///oss input && oss output
        std::ostringstream m_ossurl;
        std::ostringstream m_ossdata;
        std::ostringstream m_ossquery;
        std::ostringstream m_ossoutput;

        //
        std::string m_baseurl;

        //
        int m_reshttpcode;
        std::string m_reshttpbody;
        
};

class BulkBuilder {
    private:
        std::vector<Json::Object> operations;

        void createCommand(const std::string &op, const std::string &index, const std::string &type, const std::string &id="");

    public:
        BulkBuilder();
        void index(const std::string &index, const std::string &type, const Json::Object &fields, const std::string &id="");
        void create(const std::string &index, const std::string &type, const Json::Object &fields, const std::string &id="");
        void index(const std::string &index, const std::string &type, const Json::Object &fields);
        void create(const std::string &index, const std::string &type, const Json::Object &fields);
        void update(const std::string &index, const std::string &type, const Json::Object &body, const std::string &id="");
        void update_doc(const std::string &index, const std::string &type, const Json::Object &fields, bool update = false, const std::string &id="");
        void del(const std::string &index, const std::string &type, const std::string &id="");
        void upsert(const std::string &index, const std::string &type, const Json::Object &fields, const std::string &id="");
        void clear();
        std::string str();
        bool isEmpty();

};

#endif // ELASTICSEARCH_H
