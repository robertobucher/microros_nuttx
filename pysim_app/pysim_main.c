#include <nuttx/config.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <pysim_interfaces/msg/pysim_data.h>

#include <stdio.h>
#include <unistd.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

static rcl_subscription_t subscriber;
static rcl_publisher_t publisher;
static pysim_interfaces__msg__PysimData msgSubs;
static pysim_interfaces__msg__PysimData msgPub;

double microros_input[4];
double microros_output[4];

/* Default values */
char * nodeName = "nuttx_node";
char * subs = "nuttx_subscriber";
char * pub = "nuttx_publisher";
double pubTime = 1.0;

static void subscription_callback(const void * msgin)
{
  const pysim_interfaces__msg__PysimData * msg = (const pysim_interfaces__msg__PysimData *)msgin;
  microros_input[0] = msg->data0;
  microros_input[1] = msg->data1;
  microros_input[2] = msg->data2;
  microros_input[3] = msg->data3;
}

static void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  (void) last_call_time;
  if (timer != NULL) {
    msgPub.data0 = microros_output[0];
    msgPub.data1 = microros_output[1];
    msgPub.data2 = microros_output[2];
    msgPub.data3 = microros_output[3];
    
    RCSOFTCHECK(rcl_publish(&publisher, &msgPub, NULL));
  }
}

int main(int argc, char *argv[])
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  int opt;

  while((opt = getopt(argc, argv, "hn:p:s:t:")) != -1){
    switch(opt){
    case 'h':
      printf("-n nodeName - default is nuttx-node\n");
      printf("-p publisher Name - default is nuttx-publisher\n");
      printf("-s subscriber Name - default is nuttx-subscriber\n");
      printf("-t <publish interval>  - default is 1.0s\n");
      printf("-h -> This help\n");
      exit(1);
      break;
    case 'n':
      nodeName = strdup(optarg);
      break;
    case 'p':
      pub = strdup(optarg);
      break;
    case 's':
      subs = strdup(optarg);
      break;
    case 't':
      pubTime = atof(optarg);
      break;
    default:
	printf("No valid parameter\n");
      break;
    }
  }
  /* printf("\nNode: %s\n", nodeName); */
  /* printf("Publisher: %s\n", pub); */
  /* printf("Subscriber: %s\n", subs); */
  /* printf("dt : %lf\n", pubTime); */

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
	
  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, nodeName, "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
				      &publisher,
				      &node,
				      ROSIDL_GET_MSG_TYPE_SUPPORT(pysim_interfaces, msg, PysimData),
				      pub));

  // create timer,
  rcl_timer_t timer;
  const unsigned int timer_timeout = (const unsigned int) (pubTime*1000);
  
  RCCHECK(rclc_timer_init_default(
				  &timer,
				  &support,
				  RCL_MS_TO_NS(timer_timeout),
				  timer_callback));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
					 &subscriber,
					 &node,
					 ROSIDL_GET_MSG_TYPE_SUPPORT(pysim_interfaces, msg, PysimData),
					 subs));

  // create executor
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msgSubs, &subscription_callback, ON_NEW_DATA));

  rclc_executor_spin(&executor);
	
  // free resources
  RCCHECK(rcl_subscription_fini(&subscriber, &node));
  RCCHECK(rcl_node_fini(&node));

  return 0;
}
