![banner](.images/banner-dark-theme.png#gh-dark-mode-only)
![banner](.images/banner-light-theme.png#gh-light-mode-only)

# micro-ROS app for Nuttx 11.0 RTOS

This repository contains my version of the files for microros under the NuttX RTOS.

Differently to the version provided under https://github.com/micro-ROS/micro_ros_nuttx_app this version runs under NuttX 11 and uses UDP as transmission channel. The **colcon.meta** file under **micro_ros_lib** must be fitted with the right host address.

The applications have been tested under Humble, using a Docker version of ROS2.

## Dependencies

In order to build NuttX with the microros applications, I had to install only the colcon files. On my PC I don't have ROS2 installed!

```bash
pip3 install catkin_pkg lark-parser empy colcon-common-extensions
```

## Configuration

I've tested the system with my NUCLEO-F746ZG board with the nucleo-144:f746-pysim configuration.


```bash
./tools/configure.sh -l nucleo-144:f746-pysim
kconfig-tweak --enable CONFIG_MICROROSLIB
kconfig-tweak --enable CONFIG_MICROROS_PUBLISHER
kconfig-tweak --enable CONFIG_MICROROS_SUBSCRIBER
kconfig-tweak --enable CONFIG_MICROROS_SUM_SERVICE
make -j$(nproc)
```

## Test

Using a docker image it is possible to test the 3 provided microros applications. Launch the microros docker image by using the provided docker-compose file *docker-compose.yaml*.

```bash
docker-compose up
```

First launch the microros agent in the ROS2 docker image

```bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

The ROS2 applications are installed into the /user/ros2_ws folder of the docker images. Before starting the ROS2 specific applications we must do the following tasks. 

Open a new terminal (for example using the right mouse button. Then in the new terminal write:

```bash
cd /user/ros2_ws/src
inst
```

Then it is possible to launch the ROS2 application after starting the NuttX app:


|NuttX app  |ROS2-Docker                |
|-----------|---------------------------|
|publish    |ros2 run my_topic listener |
|subscriber |ros2 run my_topic talker   |
|sum_service|ros2 run  my_service client|

# Microros and pysimCoder under Nuttx

## Configuration

I've tested the system with my NUCLEO-F746ZG board with the nucleo-144:f746-pysim configuration.


```bash
./tools/configure.sh -l nucleo-144:f746-pysim
kconfig-tweak --enable CONFIG_MICROROSLIB
kconfig-tweak --enable CONFIG_MICROROS_PYSIM
make -j$(nproc)
make -j$(nproc) export
```

Copy the nuttx-export into pysimCoder/CodeGen/nuttx folder and recompile the devices.

It is possible now to test the code generation using the **test.dgm** file.

![banner](.images/pysim_dgm)

The G(s) block contains the line *tf(16,[1,4, 16])*, the DigOut block contains a digital output where we can connect a LED to be blinked.

## Test

After having generated and downloaded the NuttX code into the target, open the Docker image and start the microros agent.

```bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```

Now it is possible to start the NuttX applications:

```bash
nsh> pysim_intf -t0.05 &
nsh> main
```

Then simply run the publisher and the subscriber applications in ROS2

Open a new terminal and run


```bash
cd /user/ros2_ws/src
inst
ros2 run my_topic listener

```

and in an addition terminal run


```bash
cd /user/ros2_ws/src
inst
ros2 run my_topic talker
```

This is a screenshot of my PC:

![banner](.images/screen)





