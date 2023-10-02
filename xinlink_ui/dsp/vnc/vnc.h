#ifndef VNC_H
#define VNC_H

#include <core.h>
#include <stdio.h>

void * vnchost_init(XLevent_par * par);

void * vnchost(XLevent_par * par);

void * vnchost_exit(XLevent_par * par);

void * vncclient_init(XLevent_par * par);

void * vncclient(XLevent_par * par);

void * vncclient_exit(XLevent_par * par);


#endif // VNC_H
