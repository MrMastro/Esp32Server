
#ifndef InfoService_H
#define InfoService_H

#ifndef Services_H
#include <services/Service.h>
#endif

class InfoService : public Service
{
public:
    InfoService() 
    {
        
    }  // Constructor
    ~InfoService() {} // Destructor

    StatusInfo infoSuccess()
    {
        return getStatusInfoByHttpCode(HTTP_CODE::OK);
    }

    String getIp(){
        return getServerByCollector()->getIp();
    }
};

#endif // InfoService_H
