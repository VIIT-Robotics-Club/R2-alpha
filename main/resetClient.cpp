


#include <urosElement.hpp>
#include <std_srvs/srv/set_bool.h>




// Service client object
const char * service_name = "/reset";

// Get message type support
const rosidl_service_type_support_t * type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);

class resetClient : public urosElement {

public:

    std_srvs__srv__SetBool_Request req;
    std_srvs__srv__SetBool_Response res;


    // initialize hardware interface here
    resetClient(){};



    // initialize micro-ros publisher / subscribers here
    void init(){
        

        rcl_ret_t rc = rclc_client_init_default( &client, node, type_support, service_name);
        
        rclc_executor_add_client(exec, &client, &res, client_callback);
        int64_t seq;
        
        std_srvs__srv__SetBool_Request__init(&req);
        std_srvs__srv__SetBool_Response__init(&res);
        rcl_send_request(&client, &req, &seq);
    };


    static void client_callback(const void * msg){};

private:

    rcl_client_t client;
};
