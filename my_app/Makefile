############################################################################
# apps/microros/myapp/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

CFLAGS += ${shell $(INCDIR) "$(CC)" ../micro_ros_lib/include}

INCLUDE_ROS2_PACKAGES = $(shell cd ../micro_ros_lib/micro_ros_src && colcon list | awk '{print $$1}' | awk -v d=" " '{s=(NR==1?s:s d)$$0}END{print s}')
MICROROS_INCLUDES += $(foreach pkg, $(INCLUDE_ROS2_PACKAGES), ${shell $(INCDIR) "$(CC)" ../micro_ros_lib/include/$(pkg)})
CFLAGS += $(MICROROS_INCLUDES)

MAINSRC = myapp_main.c

PROGNAME = myapp
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 6000

FORMATED_CXXFLAGS := $(subst /,\/,$(CXXFLAGS))
FORMATED_CXXFLAGS := $(subst ",,$(FORMATED_CXXFLAGS))

FORMATED_CFLAGS := $(subst /,\/,$(CFLAGS))
FORMATED_CFLAGS := $(subst ",,$(FORMATED_CFLAGS))

include $(APPDIR)/Application.mk

