#ifndef DRIVERS_DHCP_H
#define DRIVERS_DHCP_H

#include <stdint.h>
#include <stddef.h>

void dhcp_init(void);
void send_dhcp_discover(void);
int handle_dhcp_offer(void);

#endif