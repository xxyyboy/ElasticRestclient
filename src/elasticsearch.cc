/**
 * @file
 * @brief
 * updated opensource by kenyuan 20200618
 */
 

#include <cassert>
#include <locale>
#include <vector>

#include "elasticsearch.h"

ElasticSearch::ElasticSearch(const std::string& node, bool readOnly): _readOnly(readOnly) {
    m_auth = false;
    m_username = "";
    m_password = "";
    m_baseurl = node;
    m_reshttpcode = -1;
    m_reshttpbody = "";
}

ElasticSearch::~ElasticSearch() {
}

void ElasticSearch::clean(){
    m_ossurl.str("");
    m_ossquery.str("");
    m_ossdata.str("");
    m_ossoutput.str("");
}

//set username & password
void ElasticSearch::setBasicAuth(const std::string username, const std::string password){
    m_username = username;
    m_password = password;
    m_auth = true;
}
        
// Request the document by index/type/id.
bool ElasticSearch::getDocument(const char* index, const char* type, Json::Object& msg, const char* id){
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/" << id;
    
    m_res = RestClient::get(m_ossurl.str(),m_username,m_password);
    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }
    
    return msg["found"];
}

// Request the document by index/type/ query key:value.
void ElasticSearch::getDocument(const std::string& index, const std::string& type, const std::string& key, const std::string& value, Json::Object& msg){
    clean();
    
    m_ossurl << m_baseurl << index << "/" << type << "/_search";
    m_ossquery << "{\"query\":{\"match\":{\""<< key << "\":\"" << value << "\"}}}";

    m_res = RestClient::post(m_ossurl.str(),m_ossquery.str(), m_username, m_password);
    //????
}

/// Delete the document by index/type/id.
bool ElasticSearch::deleteDocument(const char* index, const char* type, const char* id){
    if(_readOnly) return false;

    clean();
    
    m_ossurl << m_baseurl << index << "/" << type << "/" << id;
    
    Json::Object msg;
    
    m_res = RestClient::del(m_ossurl.str(),"", m_username,m_password);
    
    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }
    
    return msg.getValue("result");
}

/// Delete the document by index/type.
bool ElasticSearch::deleteAll(const char* index, const char* type){
    if(_readOnly) return false;
    
    clean();
    
    m_ossurl << m_baseurl << index << "/" << type << "/_query";
    m_ossquery << "{\"query\":{\"match_all\": {}}}";
    
    Json::Object msg;
    m_res = RestClient::del(m_ossurl.str(),m_ossquery.str(),m_username,m_password);
    
    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    //_http.remove(m_ossurl.str().c_str(), m_ossquery.str().c_str(), &msg);

    if(!msg.member("_indices") || !msg["_indices"].getObject().member(index) || !msg["_indices"].getObject()[index].getObject().member("_shards"))
        return false;

    if(!msg["_indices"].getObject()[index].getObject()["_shards"].getObject().member("failed"))
        return false;

    return (msg["_indices"].getObject()[index].getObject()["_shards"].getObject()["failed"].getInt() == 0);
}

// Request the document number of type T in index I.
long unsigned int ElasticSearch::getDocumentCount(const char* index, const char* type){
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/_count";
    
    Json::Object msg;
    m_res = RestClient::get(m_ossurl.str(),m_username,m_password);

    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return -1;
    }

    size_t pos = 0;
    if(msg.member("count"))
        pos = msg.getValue("count").getUnsignedInt();

    return pos;
}

// Test if document exists
bool ElasticSearch::exist(const std::string& index, const std::string& type, const std::string& id){
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/" << id;

    Json::Object msg;
    m_res = RestClient::get(m_ossurl.str(),m_username,m_password);
    
    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    return msg.getValue("found");
}

/// Index a document.
bool ElasticSearch::index(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id){
    if(_readOnly) return false;
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/" << id;
    m_ossdata << jData;

    Json::Object msg;
    m_res = RestClient::put(m_ossurl.str(),m_ossdata.str(),"application/json",m_username,m_password);

    if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 == m_reshttpcode && msg.member("created")){
        return true;
    }

    return false;
}

/// Index a document with automatic id creation
std::string ElasticSearch::index(const std::string& index, const std::string& type, const Json::Object& jData){
    if(_readOnly) return false;
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/";
    m_ossdata << jData;

    Json::Object result;
    m_res = RestClient::post(m_ossurl.str(),m_ossdata.str(),m_username,m_password);
    if(0 < m_res.body.size()){
        result.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return "";
    }

    if(!result.member("created") || !result.getValue("created")){
        return "";
    }

    return result.getValue("_id").getString();
}

// Update a document field.
bool ElasticSearch::update(const std::string& index, const std::string& type, const std::string& key, const std::string& value, const std::string& id){
    if(_readOnly) return false;
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/" << id << "/_update";
    m_ossdata << "{\"doc\":{\"" << key << "\":\""<< value << "\"}}";

    Json::Object result;
    m_res = RestClient::post(m_ossurl.str(),m_ossdata.str(),"application/json",m_username,m_password);
    if(0 < m_res.body.size()){
        result.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    if(!result.member("_version"))
        return false;

    return true;
}

// Update doccument fields.
bool ElasticSearch::update(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id){
    if(_readOnly) return false;
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/" << id << "/_update";

    std::stringstream data;
    m_ossdata << "{\"doc\":" << jData;
    m_ossdata << "}";

    Json::Object result;
    m_res = RestClient::post(m_ossurl.str(),m_ossdata.str(),"application/json",m_username,m_password);
    if(0 < m_res.body.size()){
        result.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    if(result.member("error"))
        return false;

    return true;
}

// Update or insert if the document does not already exists.
bool ElasticSearch::upsert(const std::string& index, const std::string& type, const Json::Object& jData, const std::string& id){

    if(_readOnly) return false;
    clean();

    m_ossurl << m_baseurl << index << "/" << type << "/";
    if(!id.empty()){
        m_ossurl<< id;
    }
    m_ossurl << "/_update";

    m_ossdata << "{\"doc\":" << jData;
    m_ossdata << ", \"doc_as_upsert\" : true}";
    
    RestClient::Response _res = RestClient::put(m_ossurl.str(),m_ossdata.str(),"application/json", m_username,m_password);
    Json::Object result;
    if(0 < m_res.body.size()){
        result.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    if(result.member("error"))
        return false;

    return true;
}

/// Search API of ES.
long ElasticSearch::search(const std::string& index, const std::string& type, const std::string& query, Json::Object& result){
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/_search";
    
    m_res = RestClient::post(m_ossurl.str(),m_ossdata.str(),"application/json",m_username,m_password);
        if(0 < m_res.body.size()){
        result.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return -1;
    }
    
    if(!result.member("timed_out")){
        return -1;
    }

    if(result.getValue("timed_out")){
        return -1;
    }

    return result.getValue("hits").getObject().getValue("total").getObject().getValue("value").getLong();
}

/// Delete given type (and all documents, mappings)
bool ElasticSearch::deleteType(const std::string& index, const std::string& type){
    clean();
    m_ossurl << m_baseurl << index << "/" << type;
    
    m_res = RestClient::del(m_ossurl.str(),"",m_username,m_password);
    
    return (200 == m_res.code);
}

// Test if index exists
bool ElasticSearch::exist(const std::string& index){
    clean();
    m_ossurl << m_baseurl << index << "/";
    
    m_res = RestClient::head(m_ossurl.str(),m_username,m_password);
    return (200 == m_res.code);
}

///??? kenyuan
// Create index, optionally with data (settings, mappings etc)
bool ElasticSearch::createIndex(const std::string& index, const char* data){
    m_res = RestClient::put(index,data,"application/json", m_username,m_password);
    return (200 == m_res.code);
}

// Delete given index (and all types, documents, mappings)
bool ElasticSearch::deleteIndex(const std::string& index){
    clean();
    m_ossurl << m_baseurl << index << "/";
    
    m_res = RestClient::del(m_ossurl.str(),"",m_username,m_password);
    return (200 == m_res.code);
}

// Refresh the index.
void ElasticSearch::refresh(const std::string& index){
    m_ossurl << m_baseurl << index << "/_refresh";

    Json::Object msg;
    m_res = RestClient::get(m_ossurl.str(),m_username,m_password);
    //???
}

bool ElasticSearch::initScroll(std::string& scrollId, const std::string& index, const std::string& type, const std::string& query, int scrollSize) {
    clean();
    m_ossurl << m_baseurl << index << "/" << type << "/_search?scroll=1m&search_type=scan&size=" << scrollSize;
    
    Json::Object msg;
    m_res = RestClient::post(m_ossurl.str(),m_ossdata.str(),"application/json",m_username,m_password);

    if (200 != m_res.code)
        return false;
    
        if(0 < m_res.body.size()){
        msg.addMember(m_res.body.c_str(),m_res.body.c_str()+m_res.body.size());
        m_reshttpbody = m_res.body;
    }

    m_reshttpcode=m_res.code;

    if(200 != m_reshttpcode){
        return false;
    }

    scrollId = msg["_scroll_id"].getString();
    return true;
}

bool ElasticSearch::scrollNext(std::string& scrollId, Json::Array& resultArray) {

    Json::Object msg;
    m_res = RestClient::post("/_search/scroll?scroll=1m", scrollId.c_str(), "application/json", m_username,m_password);

    if (200 != m_res.code)
        return false;

    scrollId = msg["_scroll_id"].getString();
    
    appendHitsToArray(msg, resultArray);
    return true;
}

void ElasticSearch::clearScroll(const std::string& scrollId) {
    m_res = RestClient::del("/_search/scroll","",m_username,m_password);
}

int ElasticSearch::fullScan(const std::string& index, const std::string& type, const std::string& query, Json::Array& resultArray, int scrollSize) {
    resultArray.clear();
    
    std::string scrollId;
    if (!initScroll(scrollId, index, type, query, scrollSize))
        return 0;

    size_t currentSize=0, newSize;
    while (scrollNext(scrollId, resultArray))
    {
        newSize = resultArray.size();
        if (currentSize == newSize)
            break;
        
        currentSize = newSize;
    }
    return currentSize;
}

void ElasticSearch::appendHitsToArray(const Json::Object& msg, Json::Array& resultArray) {

    if(!msg.member("hits"))
        EXCEPTION("Result corrupted, no member \"hits\".");

    if(!msg.getValue("hits").getObject().member("hits"))
        EXCEPTION("Result corrupted, no member \"hits\" nested in \"hits\".");

    for(const Json::Value& value : msg["hits"].getObject()["hits"].getArray()) {
        resultArray.addElement(value);
    }
}

// Bulk API of ES.
bool ElasticSearch::bulk(const char* data, Json::Object& jResult) {
    if(_readOnly) return false;

    m_res = RestClient::post("/_bulk", data, "application/json", m_username,m_password);

    return (200 != m_res.code);
}

BulkBuilder::BulkBuilder() {}

void BulkBuilder::createCommand(const std::string &op, const std::string &index, const std::string &type, const std::string &id) {
    Json::Object command;
    Json::Object commandParams;

    if (id != "") {
        commandParams.addMemberByKey("_id", id);
    }

    commandParams.addMemberByKey("_index", index);
    commandParams.addMemberByKey("_type", type);

    command.addMemberByKey(op, commandParams);
    operations.push_back(command);
}

void BulkBuilder::index(const std::string &index, const std::string &type, const Json::Object &fields, const std::string &id) {
    createCommand("index", index, type, id);
    operations.push_back(fields);
}

void BulkBuilder::create(const std::string &index, const std::string &type, const Json::Object &fields, const std::string &id) {
    createCommand("create", index, type, id);
    operations.push_back(fields);
}

void BulkBuilder::index(const std::string &index, const std::string &type, const Json::Object &fields) {
    createCommand("index", index, type);
    operations.push_back(fields);
}

void BulkBuilder::create(const std::string &index, const std::string &type, const Json::Object &fields) {
    createCommand("create", index, type);
    operations.push_back(fields);
}

void BulkBuilder::update(const std::string &index, const std::string &type, const Json::Object &body, const std::string &id) {
    createCommand("update", index, type, id);
    operations.push_back(body);
}

void BulkBuilder::update_doc(const std::string &index, const std::string &type, const Json::Object &fields, bool upsert, const std::string &id) {
    createCommand("update", index, type, id);

    Json::Object updateFields;
    updateFields.addMemberByKey("doc", fields);
    updateFields.addMemberByKey("doc_as_upsert", upsert);

    operations.push_back(updateFields);
}

void BulkBuilder::del(const std::string &index, const std::string &type, const std::string &id) {
    createCommand("delete", index, type, id);
}

std::string BulkBuilder::str() {
    std::stringstream json;

    for(auto &operation : operations) {
        json << operation.str() << std::endl;
    }

    return json.str();
}

void BulkBuilder::clear() {
    operations.clear();
}

bool BulkBuilder::isEmpty() {
    return operations.empty();
}
