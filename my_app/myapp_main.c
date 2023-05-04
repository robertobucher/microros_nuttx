#include <nuttx/config.h>
#include <nuttx/ioexpander/gpio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

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

static rcl_subscription_t subscriber;
static std_msgs__msg__Int32 msg;
static int fd;

/* Default values */
static char * gpioPort = "/dev/gpio0";
static char * subs = "nuttx_subscriber";
static char * nodeName = "nuttx_node";

static void subscription_callback(const void * msgin)
{
  bool outvalue;
  int ret;
  
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  /* printf("Received: %d\n", msg->data); */
  if(msg->data==1) outvalue = true;
  else outvalue = false;
  
  ret = ioctl(fd, GPIOC_WRITE, (unsigned long) outvalue);
  if (ret < 0){
    fprintf(stderr,"ERROR: Failed to write value to %s\n", gpioPort);
    close(fd);
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  int opt;
  int ret;
  enum gpio_pintype_e pintype;

  while((opt = getopt(argc, argv, "hd:n:s:")) != -1){
    switch(opt){
    case 'h':
      printf("-d GPIO device - default is /dev/gpio0\n");
      printf("-n nodeName - default is nuttx-node\n");
      printf("-s subscriber Name - default is nuttx-subscriber\n");
      printf("-h -> This help\n");
      exit(1);
      break;
    case 'n':
      nodeName = strdup(optarg);
      break;
    case 'd':
      gpioPort = strdup(optarg);
      break;
    case 's':
      subs = strdup(optarg);
      break;
    default:
	printf("No valid parameter\n");
      break;
    }
  }

  // Open device
  fd = open(gpioPort, O_RDWR);
  
  if (fd < 0){
    fprintf(stderr, "ERROR: Failed to open %s\n", gpioPort);
    exit(1);
  }
  ret = ioctl(fd, GPIOC_PINTYPE, (unsigned long)((uintptr_t)&pintype));
  if (ret < 0){
    fprintf(stderr, "ERROR: Failed to read pintype from %s\n", gpioPort);
    close(fd);
    exit(1);
  }

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
	
  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, nodeName, "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
					 &subscriber,
					 &node,
					 ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
					 subs));

  // create executor
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));

  rclc_executor_spin(&executor);
	
  // free resources
  RCCHECK(rcl_subscription_fini(&subscriber, &node));
  RCCHECK(rcl_node_fini(&node));

  return 0;
}
