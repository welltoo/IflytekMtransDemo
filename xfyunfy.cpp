/*这三个参数需要自己去讯飞开放平台申请，并在控制台中查看，本代码第62行、78行、147行需要对应修改为自己申请的*/
#define your_apisecret 7980a7093abf7fb65b6b0a9335944758
#define your_apikey 62adb0be6a8246e7c5f6cca1070b3894
#define your_appid 5e05ca47

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "http_time.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"
#include "Poco/Base64Encoder.h"

#include "Poco/Crypto/Crypto.h"
#include "Poco/Crypto/DigestEngine.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/StreamCopier.h"
#include "Poco/String.h"


#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iterator>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::WebSocket;
using Poco::Net::NetException;

using Poco::Crypto::DigestEngine;

using Poco::StreamCopier;
using Poco::SHA1Engine;

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Crypto;
//using namespace Poco::JSON;
#include "Poco/SHA2Engine.h"

#include <Poco/Crypto/DigestEngine.h>

const char* APISECRET = "7980a7093abf7fb65b6b0a9335944758";


class SHA256Engine : public Poco::Crypto::DigestEngine{
public:
    enum{
        BLOCK_SIZE = 64,
        DIGEST_SIZE = 32
    };

    SHA256Engine()
            : DigestEngine("SHA256"){
    }
};
string p_body = "{\n"
                   "    \"common\": {\n"
                   "        \"app_id\": \"5e05ca47\"\n"
                   "    },\n"
                   "    \"business\": {\n"
                   "        \"from\": \"cn\",\n"
                   "                \"to\": \"en\"\n"
                   "    },\n"
                   "    \"data\": {\n"
                   "        \"text\": \"56eR5aSn6K6v6aOe5py65Zmo57+76K+R5o6l5Y+j\"\n"
                   "    }"
                   "}";
string get_signature_origin(const char* host, const char* date, const  char* request_line, const char* digest=""){
    stringstream ss;
    ss <<"host: "<< host << "\n" << "date: " << date << "\n" << request_line<<"\n"<<digest;

    cout << "get_signature_origin | " << ss.str() << std::endl;

    return ss.str();
}

string vec_uchar_to_old_base64string(std::vector < unsigned char >& vc){
    std::stringstream ss;
    Poco::Base64Encoder encoder(ss);
    std::copy(vc.begin(), vc.end(), std::ostream_iterator<unsigned char>(encoder));
    encoder.close();

    std::string s(Poco::replace(ss.str(), "\r\n", ""));

    return s;
}


int http_post(){

    printf("start to datetime\n");
    int buf_len = 100;
    char buf[buf_len] = {0};

    time_t t;
    time(&t);
    make_http_time(t, buf, buf_len);
    std::string dt ;
    dt = buf;
    std::string header_digest;
    std::string p_header_digest;
    std::string encryptHexText;
    // 1.0 计算出来的值,进行SHA256编码
    SHA256Engine hmac;
    hmac.update(p_body);
    Poco::DigestEngine::Digest digest = hmac.digest();

    int flen = 0;
   // 1.1 将上一步的计算出来的值,进行base64编码,特别注意这里的输入上一步骤hmac sha计算出来的二进制数据
    string string_b64 = vec_uchar_to_old_base64string(digest);
    std::cout << "get_signa|" << "4. bas64 : " << string_b64 << std::endl;


    header_digest = string("digest: SHA-256=") + string_b64.c_str();
    p_header_digest=string("SHA-256=")+string_b64.c_str();

   //1.2 sha256计算
    std::string signature_origin = get_signature_origin("itrans.xfyun.cn", dt.c_str(), "POST /v2/its HTTP/1.1", header_digest.c_str());
    Poco::HMACEngine<SHA256Engine> hmac2{APISECRET};
    hmac2.update(signature_origin);
    Poco::DigestEngine::Digest digest2 = hmac2.digest();
   //1.3进行base64编码,特别注意这里的输入上一步骤hmac sha256计算出来的二进制数据
    string sha256_s = vec_uchar_to_old_base64string(digest2);
    std::cout << "get_signa|" << "1.3. bas64 : " << sha256_s << std::endl;

    //2.0得到author
    std::string p_author = "api_key=\"62adb0be6a8246e7c5f6cca1070b3894\", algorithm=\"hmac-sha256\", headers=\"host date request-line digest\", signature=\"";
    p_author=p_author+sha256_s+ "\"";
    //3.0设置header
    map<string,string> p_headers;
    p_headers["Content-Type"]="application/json";
    p_headers["Accept"]="application/json";
    p_headers["Date"]=buf;
    p_headers["Digest"]=p_header_digest;
    p_headers["Authorization"]=p_author;

try{
    Poco::URI url("http://itrans.xfyun.cn/v2/its");
    HTTPClientSession session(url.getHost(),url.getPort());
    HTTPRequest req(HTTPRequest::HTTP_POST,url.getPathAndQuery(),HTTPRequest::HTTP_1_1);
    //set headers here
    for(map<string,string>::iterator it = p_headers.begin();it != p_headers.end(); it++){
	    req.set(it->first,it->second);
    }
    //set the request body
    req.setContentLength(p_body.length());
    //sent req,returns open stream
    std::ostream& os = session.sendRequest(req);
    os<<p_body;
    HTTPResponse res;

    cout<<res.getStatus()<<"\t"<<res.getReason()<<endl;
    istream &is = session.receiveResponse(res);
    stringstream ss;
    StreamCopier::copyStream(is, ss);

  
    string g_p=ss.str();
    //string g_p="ss.str()";
   
    JSON::Parser parser;
    Dynamic::Var result;
    parser.reset();
    result = parser.parse(string(g_p));
    JSON::Object::Ptr pObj = result.extract<JSON::Object::Ptr>();
    Dynamic::Var ret = pObj->get("data");   
    std::cout << "---------------------------------------------------------------------" << std::endl;
        Poco::JSON::Object::Ptr data_obj = ret.extract<Poco::JSON::Object::Ptr>();        
        Poco::Dynamic::Var dataresult = data_obj->get("result");
    std::cout << "---------------------------------------------------------------------" << std::endl;
        Poco::JSON::Object::Ptr trans_result_obj = dataresult.extract<Poco::JSON::Object::Ptr>(); 
        Poco::Dynamic::Var trans_result = trans_result_obj->get("trans_result");
     std::cout << "---------------------------------------------------------------------" << std::endl;
        Poco::JSON::Object::Ptr dstsrc_obj = trans_result.extract<Poco::JSON::Object::Ptr>(); 
        Poco::Dynamic::Var dstresult = dstsrc_obj->get("dst");
        Poco::Dynamic::Var srcresult = dstsrc_obj->get("src");
        if ( dstresult.isEmpty()||srcresult.isEmpty() ) {
    	   std::cout << "is null" << std::endl;
        } else {
    	     std::cout <<"翻译结果: "<<dstresult.toString() << std::endl;
             std::cout <<"原始字符: "<< srcresult.toString() << std::endl;
        }

}
catch(NetException & ex){
    std::cout<<"net bad"<<endl;
    std::cout<<"bad: "<<ex.what()<<endl;
    std::cout << ex.displayText();          
}
catch(Poco::InvalidAccessException & ex){
    std::cout<<"access bad"<<endl;
    std::cout<<"invalida access: "<<ex.what()<<endl;
    std::cout << ex.displayText();          
}
catch(Poco::JSON::JSONException & jx){
    std::cout<<"json解析出错"<<endl;
}
catch(...){
    std::cout<<"网络和json解析未出错，其他出错"<<endl;
}

 }

int main(int argc, char *argv[]) {

  
    http_post();

}



