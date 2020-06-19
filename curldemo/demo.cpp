#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string>
#include <string.h>

using namespace std;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    strncat((char *)stream, (char *)ptr, size*nmemb);
    return size * nmemb;
}

int esclient(const string &_http, const string &_json, const string &_command)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist* headers = NULL;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_USERPWD, "elastic:password");
    curl_easy_setopt(curl, CURLOPT_URL, _http.c_str());
    headers=curl_slist_append(headers, "Content-Type:application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    char out[40960]={0};
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

    if(_command=="delete")
    {
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else if(_command=="get")
    {
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST, "GET");
    }
    else
    {
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST, "POST");
    }

    if(!_json.empty())
    {
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,_json.c_str());
        curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,_json.length());
    }

    res = curl_easy_perform(curl);
    
    printf("%s\n",out);
    curl_easy_cleanup(curl);
    return res;
}

int main(void)
{
    esclient("http://127.0.0.1:9200/customer/external/1","","get");//查询单条数据
    printf(" TEST \n \n \n");
    esclient("http://127.0.0.1:9200/bank/_search","","search");//批量查询数据，但是只返回10条，如需其他要求，加json语句
    printf(" TEST \n \n \n");
    esclient("http://127.0.0.1:9200/customer/external/3/"," { \"name\": \"hhh\" }","add");//增加一条数据
    printf(" TEST \n \n \n");
    esclient("http://127.0.0.1:9200/customer/external/2/_updata/"," {\"doc\": { \"name\": \"eee\" }}","updata");//更新某条数据
    printf(" TEST \n \n \n");
    esclient("http://127.0.0.1:9200/ccc","","delete");//删除索引
    printf(" TEST \n \n \n");
    esclient("http://127.0.0.1:9200/customer/external/_delete_by_query"," {\"query\":{ \"match\":{\"name\": \"hhh\" }}}","");//匹配删除数据
    printf(" TEST \n \n \n");
}

