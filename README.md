# C++ WebSocket Server skeleton project

English | [简体中文](./README_cn.md)

This project is a C++ WebSocket service skeleton project, based on the websocketpp + unzip library, to implement front-end and back-end integration of WebSocket services + static HTTP file service (similar to the foreground file embedded in the service program runtime file in golang).
The service program only supports Linux systems (the build system only supports Linux).

## 1. Directory hierarchy

The directories of the front and back offices are managed separately, in which the foreground code is concentrated in the frontend directory, and the molecular project is managed; The background service code is managed in the backend directory.

### 1.1 frontend directory

In each foreground subproject directory, there must be a dist directory, and the contents of this directory store the foreground final code that will be packaged into the server program running file (which will be zip compressed)

### 1.2 backend directory

Under the background directory, it is divided into 3rd, bin, inc, lib, src and other directories

#### 1.2.1 3rd directory

This directory stores third-party libraries that background services rely on, such as websocketpp, asio and unzip.

#### 1.2.2 bin directory

This directory holds the compiled service program

#### 1.2.3 inc directory

This directory holds header files that the server relies on publicly, mainly the web_assets_xxx.h file packaged by the foreground program.

#### 1.2.4 lib directory

This directory holds compiled library files or library files from third-party libraries

#### 1.2.5 src directory

The main directory of the background service, each subproject is stored in a directory, of which the common directory stores some library code shared by each subproject, which will be compiled into library files, and the rest of the directories should be compiled into service program running files.

## 1.3 Compile

Run under the root directory
```
make
```
The corresponding service program is generated.

If debugging is required, it should be run
```
make BUILD_TYPE=Debug
```
Build a service program with debugging information

## 1.4 Current skeleton project

In the current project, the wspp project is finished. When the project compile & link is finished, and running it. it will listen in 8080 and access the http://127.0.0.1:8080/static/ to view the service content.