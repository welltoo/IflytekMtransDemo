## 					C++版“机器翻译”WebAPI的Demo详解

#### 一：概况

​		[讯飞开放平台](https://www.xfyun.cn/) 提供大量的AI能力，并提供了java和python3等多种语言的调用Demo，下载即用，方便快捷，瞬间便可实现诸如翻译、转写等人工智能能力！官方Demo虽好，但是它并未提供C++版本，为了使用更加方便，本文针对“机器翻译”能力用C++写了一个调用的WebAPI的Demo,分享出来与大家交流学习。

​		本文分为五个部分，详细讲解如何从阅读WebAPI开发文档到最终实现过程。文档中需要以POST形式进行通信，其中所有参数都在HTTP请求的头部，所以Demo主要工作是生成请求的头部即可。本文第三章中需要的参数将在第四章介绍其生成过程。全文采用自顶向下的思路来介绍，并给出了所有代码原文。文中出现的APPID、APISecret以及APIKey读者可以登录讯飞开放平台控制台查看。

#### 二：Demo准备过程

#####    		 **POCO简介：**

​		使用POCO库（https://pocoproject.org） 进行Demo的编写，POCO是一个适合编写后台处理程序的C++库（开源、高效、可移植、商业化免费、模块化、跨平台以及简易可读...），此外POCO拥有内存占用量低的同时性能表现优秀的特性，其完整的框架可大幅减少研发工作，被越来越多的人使用。

​		POCO库提供Zip压缩、网络通信、Crypto加密、数据库等开发接口，可以完成大部分开发工作。POCO的概况如下图所示：

![1584687820_1_.png](https://i.loli.net/2020/03/20/jgt3H5MESU2vdFq.png)

##### 		    **安装环境：**

​		使用ubuntu18.04系统，使用Poco 1.10.0源码（网上搜索poco安装教程，总会装上的）。简要步骤如下：

1. 安装g++和cmake；

   ```shell
   sudo apt install g++
   sudo apt install cmake
   ```

2. 编译Poco

   参考poco官网https://pocoproject.org  也可自行搜索相关博客。

   ##### **执行代码：**

   本文在最后一章给出了所有代码。其中部分代码需要修改，列表如下：

1. 修改参数。本文给出了CMakeLists.txt的内容，其中两个变量需要设置，如下所示,其中POCO_PATH为你下载的poco源码路径，POCO_LIB_PATH是你安装后库的路径，如果选择默认安装位置则POCO_LIB_PATH无需修改：

   ```cmake
   set(POCO_PATH "/home/xxxx/iflytek/poco-1.10.0/Poco")
   set(POCO_LIB_PATH "usr/local/lib")
   ```

2. 执行。先执行 `cmake .` ,之后执行  `make` ，则可生成可执行文件xfyunfy,执行该文件 `./xfyunfy` 执行结果就会显示在屏幕上。最终结果如下图所示：

   ![1584688777_1_.png](https://i.loli.net/2020/03/20/WmFndDzEVq81H7R.png)

#### 三：完成网络通信并解析JSON结果，捕获异常

**第一步：**首先设置url，然后利用POCO声明一个HTTP客户端(HTTPClientSession),最后发送一个请求(HTTPRequest)，完成一个请求过程。

```c++
Poco::URI url("http://itrans.xfyun.cn/v2/its");
    HTTPClientSession session(url.getHost(),url.getPort());
    HTTPRequest req(HTTPRequest::HTTP_POST,url.getPathAndQuery(),HTTPRequest::HTTP_1_1);
```

**第二步：**上一步操作已经可以和服务器进行连接，但是请求参数和鉴权参数在Http Request Header中，所以还需要设置Header才能正确得到服务器返回值。如下设置Header为p_header,其中p_header为所需参数。P_header具体在下章介绍。

```c++
//set headers here
    for(map<string,string>::iterator it = p_headers.begin();it != p_headers.end(); it++){
    	req.set(it->first,it->second);
    }
```

**第三步：**设置Http Request Body，如下所示，其中p_body为所需参数，在下一章具体介绍。

```c++
    //set the request body
    req.setContentLength(p_body.length());
    //sent req,returns open stream
    std::ostream& os = session.sendRequest(req);
    os<<p_body;
```

**第四步：**得出结果，将服务器返回的json格式数据输出到屏幕，如下所示。

```c++
    cout<<"\n返回信息 "<<endl;
    HTTPResponse res;
    cout<<res.getStatus()<<"\t"<<res.getReason()<<endl;
    istream &is = session.receiveResponse(res);
    stringstream ss;
    StreamCopier::copyStream(is, ss);
    cout<<endl<<ss.str()<<endl<<endl<<endl;
```

**第五步：**解析json格式，并输出我们需要的部分，服务器返回的json数据格式如下，我们需要解析出dst和src部分的内容。具体解析过程如下代码，其中关键步骤解释已在代码中注释。

```json
{
  "code": 0,
  "message": "success",
  "sid": "its....",
  "data": {
    "result": {
      "from": "cn",
      "to": "en",
      "trans_result": {
        "dst": "Hello World ",
        "src": "你好世界"
      }
    }
  }
}
```

```c++
    string g_p=ss.str();
    JSON::Parser parser;
    Dynamic::Var result;
    parser.reset();
    result = parser.parse(string(g_p));
//得出jsaon数据中的data部分
    JSON::Object::Ptr pObj = result.extract<JSON::Object::Ptr>();
    Dynamic::Var ret = pObj->get("data");  
//得出data里的result部分
    Poco::JSON::Object::Ptr data_obj = ret.extract<Poco::JSON::Object::Ptr>();       
    Poco::Dynamic::Var dataresult = data_obj->get("result");
//得出data里的result里的trans_result部分
    Poco::JSON::Object::Ptr trans_result_obj = dataresult.extract<Poco::JSON::Object::Ptr>(); 
    Poco::Dynamic::Var trans_result = trans_result_obj->get("trans_result");
//得出data里的result里的trans_result里的dst和src部分
    Poco::JSON::Object::Ptr dstsrc_obj = trans_result.extract<Poco::JSON::Object::Ptr>(); 
    Poco::Dynamic::Var dstresult = dstsrc_obj->get("dst");  
	Poco::Dynamic::Var srcresult = dstsrc_obj->get("src");
    if ( dstresult.isEmpty()|| dstresult.isEmpty()) {
       std::cout << "is null" << std::endl;
    } else {
         std::cout<< "翻译结果: "<<dstresult.toString() << std::endl;
         std::cout<< "原始字符: "<<srcresult.toString() << std::endl;
    }
```

**第六步：**捕获异常，首先捕获网络异常，捕获鉴权参数异常，之后捕获json解析异常，最后捕获其他异常，代码如下：

```c++
try{"此处为第一步到第五步的代码"}
catch(NetException & ex){
    std::cout<<"net bad"<<endl;
    std::cout << ex.displayText();          
}
catch(Poco::InvalidAccessException & ex){
    std::cout<<"access bad"<<endl;
    std::cout << ex.displayText();          
}
catch(Poco::JSON::JSONException & jx){
    std::cout<<"json解析出错"<<endl;
}
catch(...){
    std::cout<<"网络和json解析未出错，其他出错"<<endl;
}

```

#### 四：生成第三步所需参数

写在前面：本部分采用自顶向下的思路，先介绍一个目标参数，再介绍生成目标参数所需参数，一层层向下解释。

##### **1. 生成headers参数**

```c++
map<string,string> p_headers;
p_headers["Content-Type"]="application/json";
p_headers["Accept"]="application/json";
p_headers["Date"]=buf;
p_headers["Digest"]=p_header_digest;
p_headers["Authorization"]=p_author;
```

**“Content-Type”和“Accept”：**根据开发文档设置的固定值,

**“Date”：**当前系统时间，Date的时间格式需要使用RFC1123格式，相关时间转换代码见第六章相关代码http_time部分。

**“Digest”：**Base64(SHA256(请求body))的结果，还需要在其前面加入"SHA-256="。组成“SHA-256=Base64(SHA256(请求body))”字段，代码中的p_header_digest参数是

**“Authorization”：**格式为：api_key="your_key", algorithm="hmac-sha256", headers="host date request-line digest", signature="$signature"。以下代码中的p_author变量是Authorization参数，代码中"your_apikey\"为申请的api_key，algorithm和headers是根据开发文档设定的固定值，sha256_s为开发文档的signature参数（在下一节介绍）。

```c++
//2.0得到author
std::string p_author = "api_key=\"your_apikey\", algorithm=\"hmac-sha256\", headers=\"host date request-line digest\", signature=\"";
p_author=p_author+sha256_s+ "\"";
```

##### 2 **生成signature参数**

**2.1 signature**=base64(signature_sha)；代码中sha256_s变量是signature参数，vec_uchar_to_old_base64string（）是base64的编码函数，digest2为上一步sha256的结果（在下一节介绍）。注意：这里输入上一步骤hmac sha256计算出来的**二进制数据**、**二进制数据**！

```c++
 //1.3进行base64编码,特别注意这里的输入上一步骤hmac sha256计算出来的二进制数据
string sha256_s = vec_uchar_to_old_base64string(digest2);
std::cout << "get_signa|" << "1.3. bas64 : " << sha256_s <<std::endl;
```

```c++

string vec_uchar_to_old_base64string(std::vector<unsigned char>& vc){
    std::stringstream ss;
    Poco::Base64Encoder encoder(ss);
    std::copy(vc.begin(), vc.end(), std::ostream_iterator<unsigned char>(encoder));
    encoder.close();
    std::string s(Poco::replace(ss.str(), "\r\n", ""));
    return s;
}
```

**2.2 digest2**=hmac-sha256(signature_origin,$apiSecret)；代码中如下，其中signature_origin参数是根据开发文档生成的，生成函数为get_signature_origin（参数），如下给出。APISECRET为从讯飞开放平台申请的，函数参数dt.c_str()是时间参数，header_digest.c_str()为上一步计算出来的头部参数（下一小节介绍）。

```c++
//1.2 sha256计算
std::string signature_origin = get_signature_origin("itrans.xfyun.cn", dt.c_str(), "POST /v2/its HTTP/1.1", header_digest.c_str());
Poco::HMACEngine<SHA256Engine> hmac2{APISECRET};
hmac2.update(signature_origin);
Poco::DigestEngine::Digest digest2 = hmac2.digest();
```

```c++
string get_signature_origin(const char* host, const char* date, const  char* request_line, const char* digest=""){
    stringstream ss;
    ss <<"host: "<< host << "\n" << "date: " << date << "\n" << request_line<<"\n"<<digest;
    cout << "get_signature_origin | " << ss.str() << std::endl;
    return ss.str();
}
```

**2.3header_digest**为digest: SHA-256=Base64(SHA256(请求body))，body将在下一小节介绍

```c++
 // 1.0 body进行SHA256编码
    SHA256Engine hmac;
    hmac.update(p_body);
    Poco::DigestEngine::Digest digest = hmac.digest();
    int flen = 0;
```

```c++
// 1.1 将上一步1.0步骤计算出来的值,进行base64编码,特别注意这里的输入上一步骤hmac sha计算出来的二进制数据
    string string_b64 = vec_uchar_to_old_base64string(digest);
    std::cout << "get_signa|" << "4. bas64 : " << string_b64 << std::endl;
    header_digest = string("digest: SHA-256=") + string_b64.c_str();
```

**2.4body**为根据开发文档生成，body的组成如下，直接构造即可。其中text的值为：要翻译的文本进行base64编码的结果，可以直接网络搜索base64转换网站进行转换。

```c++

string p_body = "{\n"
                   "    \"common\": {\n"
                   "        \"app_id\": \"5ddb517c\"\n"
                   "    },\n"
                   "    \"business\": {\n"
                   "        \"from\": \"cn\",\n"
                   "                \"to\": \"en\"\n"
                   "    },\n"
                   "    \"data\": {\n"
                   "        \"text\": \"5LuK5aSp5aSp5rCU5oCO5LmI5qC377yf\"\n"
                   "    }"
                   "}";
```







