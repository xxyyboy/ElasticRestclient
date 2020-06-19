/*
 * Licensed to cpp-elasticsearch under one or more contributor
 * license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright
 * ownership. Elasticsearch licenses this file to you under
 * the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Basic use cpp-elasticsearch library
*/

#include <iostream>
#include "elasticsearch.h"

int main(int, char**) {

    // Instanciate elasticsearch client.
    ElasticSearch es("http://127.0.0.1:9200/");
    es.setBasicAuth("elastic","password");
    
    // Index one document.
    Json::Object jData;
    jData.addMemberByKey("user", "kimchy");
    jData.addMemberByKey("post_date", "2009-11-15T14:12:12");
    jData.addMemberByKey("message", "trying out Elasticsearch");

    if(!es.index("twitter", "tweet", jData, "1"))
        std::cerr << "Index failed." << std::endl;

    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;

    // Get document.
    Json::Object jResult;
    if(!es.getDocument("twitter", "tweet", jResult, "1"))
        std::cerr << "Failed to get document." << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;


    if(jResult["_source"].getObject() != jData)
        std::cerr << "Oups, something did not work." << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;


    std::cout << "Great we indexed our first document: " << jResult.pretty() << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;
    
    // Update document
    Json::Object jUpdateData;
    jUpdateData.addMemberByKey("user", "cpp-elasticsearch");
    if(!es.update("twitter", "tweet", jUpdateData, "1"))
        std::cerr << "Failed to update document." << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;


    // Search document.
    Json::Object jSearchResult;
    long resultSize = es.search("twitter", "tweet", "{\"query\":{\"match_all\":{}}}", jSearchResult);
    std::cout << "We found " << resultSize << " result(s):\n" << jSearchResult.pretty() << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;

    // Delete document.
    if(!es.deleteDocument("twitter", "tweet", "1"))
        std::cerr << "Failed to delete document." << std::endl;
    std::cout<<"httpcode:"<<es.httpcode()<<std::endl;
    std::cout<<"httpbody:"<<es.httpbody()<<std::endl;


    std::cout << "First test is over. Good Bye." << std::endl;
  
    return EXIT_SUCCESS;
}

