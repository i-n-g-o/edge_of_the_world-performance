#include "m_pd.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static t_class *ipinfo_class;

typedef struct _ipinfo {
  t_object x_obj;
  t_outlet *x_listout;
} t_ipinfo;


static void get_ip(t_ipinfo *x, char* name)
{
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1)
  {
      error("getifaddrs");
      return;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
      if (ifa->ifa_addr == NULL)
          continue;

      s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

      if((ifa->ifa_addr->sa_family==AF_INET))
      {
          if (name == NULL || (strcmp(ifa->ifa_name,name)==0))
          {
            if (s != 0)
            {
                error("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }

            t_atom      *list  = (t_atom *)getbytes(sizeof(t_atom)*2);
            if (list != NULL)
            {
              SETSYMBOL(&list[0], gensym(ifa->ifa_name));
              SETSYMBOL(&list[1], gensym(host));

              outlet_list(x->x_listout, &s_list, 2, list);
              freebytes((void *)list, sizeof(t_atom)*2);
            }
          }
      }
  }

  freeifaddrs(ifaddr);
}

static void ipinfo_bang(t_ipinfo *x)
{
  (void)x; // silence unused variable warning

  get_ip(x, NULL);
}

static void ipinfo_doanything(t_ipinfo *x, t_symbol *s, int argc, t_atom *argv)
{
  if(s == gensym(""))
  {
      return;
  }

  get_ip(x, s->s_name);
}

void *ipinfo_new(void)
{
  t_ipinfo *x = (t_ipinfo *)pd_new(ipinfo_class);

  x->x_listout = outlet_new(&x->x_obj, &s_list);

  return (void *)x;
}

void ipinfo_setup(void)
{
  ipinfo_class = class_new(gensym("ipinfo"),
    (t_newmethod)ipinfo_new, NULL,
    sizeof(t_ipinfo), CLASS_DEFAULT, 0);

  class_addbang(ipinfo_class, ipinfo_bang);
  class_addanything(ipinfo_class, ipinfo_doanything);
}
