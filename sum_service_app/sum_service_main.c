#include <nuttx/config.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <pysim_interfaces/srv/somma.h>

#include <stdio.h>
#include <unistd.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

void service_callback(const void * req,  void * res)
{
  pysim_interfaces__srv__Somma_Request * req_in = (pysim_interfaces__srv__Somma_Request *) req;
  pysim_interfaces__srv__Somma_Response * res_in = (pysim_interfaces__srv__Somma_Response *) res;
  printf("Service request value: %d + %d.\n",  (int) req_in->a, (int) req_in->b);
  res_in->somma = req_in->a + req_in->b;
}

int main(int argc, char *argv[])
{
   rcl_allocator_t allocator = rcl_get_default_allocator();

   // create init_options
   rclc_support_t support;
   rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();

  
   RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

   // create node
    rcl_node_t node;
    RCCHECK(rclc_node_init_default(&node, "sum_values_node", "", &support));

    // create service
    rcl_service_t service;
    RCCHECK(rclc_service_init_default(&service, &node, ROSIDL_GET_SRV_TYPE_SUPPORT(pysim_interfaces, srv, Somma), "/sum_values"));

    // create executor
    rclc_executor_t executor;
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

    pysim_interfaces__srv__Somma_Response res;
    pysim_interfaces__srv__Somma_Request req;

    RCCHECK(rclc_executor_add_service(&executor, &service, &req, &res, service_callback));

    rclc_executor_spin(&executor);

    RCCHECK(rcl_service_fini(&service, &node));
    RCCHECK(rcl_node_fini(&node));

  return 0;
}
