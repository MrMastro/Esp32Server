
#ifndef InfoService_H
#define InfoService_H

#ifndef Services_H
#include <services/Service.h>
#endif

class InfoService : public Service
{
public:
    InfoService() {}  // Constructor
    ~InfoService() {} // Destructor

    // String executeJson(String methodName, String param) override
    // {
    //     if (methodName == "jsonPong")
    //     {
    //         return jsonPong();
    //     }
    //     else if (methodName == "jsonInfoSuccess")
    //     {
    //         return jsonInfoSuccess();
    //     }
    //     else if (methodName == "getIp")
    //     {
    //         return "workInProgress";
    //     }
    //     else
    //     {
    //         return "Service Method not found";
    //     }
    // }

    // String executeJson(String methodName, std::vector<String> jsonParams) override
    // {
    //     return executeJson(methodName,"");
    // }

    String jsonPong()
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
        BasicResponse response = BasicResponse(s);
        return dtoToJson(response);
    }

    String jsonInfoSuccess()
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
        BasicResponse response = BasicResponse(s);
        return dtoToJson(response);
    }

    // String getIp(){
    //     return
    // }
};

#endif // InfoService_H
