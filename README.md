# ElasticRestclient
Elasticsearch C++ API, using libcurl, Restclient 
support basicAuth using username and password


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
