#include <nuttx/config.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/int32.h>

#include <stdio.h>
#include <unistd.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

static rcl_publisher_t publisher;
static std_msgs__msg__Int32 msg;

static void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  (void) last_call_time;
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    printf("Sent: %d\n", msg.data);
    msg.data++;
  }
}

int main(int argc, char *argv[])
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
	
  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, "nuttx_int32_publisher", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
				      &publisher,
				      &node,
				      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
				      "nuttx_int32_publisher"));

  // create timer,
  rcl_timer_t timer;
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
				  &timer,
				  &support,
				  RCL_MS_TO_NS(timer_timeout),
				  timer_callback));

  // create executor
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  msg.data = 0;

    while(1){
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    usleep(100000);
    }
	
  // free resources
  RCCHECK(rcl_publisher_fini(&publisher, &node))
    RCCHECK(rcl_node_fini(&node))

    
    return 0;
}
