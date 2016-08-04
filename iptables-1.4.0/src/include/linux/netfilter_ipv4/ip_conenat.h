#ifndef __IP_CONE_NAT_H__
#define __IP_CONE_NAT_H__

struct ip_nat_multi_range;

struct ip_cone_nat_data
{
    int cone_nat_type;
    struct ip_nat_multi_range data;
};

// CONE NAT 三种类型，full cone/addr restricted cone/port restricted cone
enum cone_nat_type
{
    FULL_CONE_TYPE = 0,
    ADDR_CONE_TYPE = 1,
    PORT_CONE_TYPE = 2,    
    CONE_NAT_TYPE_MAX,
};

#endif
