#include "m_pd.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

static t_class *ether_class;

typedef struct _ether {
  t_object x_obj;
  t_outlet *x_symout;
} t_ether;


static void get_mac(t_ether *x, char* name)
{

#ifdef __linux__

  int s;
  char macaddr[18];
  macaddr[17] = 0;

	struct ifreq buffer;
  memset(&buffer, 0x00, sizeof(buffer));

	s = socket(PF_INET, SOCK_DGRAM, 0);

	strcpy(buffer.ifr_name, name);

	ioctl(s, SIOCGIFHWADDR, &buffer);

	close(s);

  // output
  sprintf(macaddr, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
            (unsigned char)buffer.ifr_hwaddr.sa_data[0],
            (unsigned char)buffer.ifr_hwaddr.sa_data[1],
            (unsigned char)buffer.ifr_hwaddr.sa_data[2],
            (unsigned char)buffer.ifr_hwaddr.sa_data[3],
            (unsigned char)buffer.ifr_hwaddr.sa_data[4],
            (unsigned char)buffer.ifr_hwaddr.sa_data[5]);

  t_symbol *sym = gensym(macaddr);

  outlet_symbol(x->x_symout, sym);

  return;

#elif defined(__APPLE__)

  // TODO

#elif defined(_WIN32)

  // TODO

#endif

  post("not implemented");
  
}

static void ether_doanything(t_ether *x, t_symbol *s, int argc, t_atom *argv)
{
  if(s == gensym(""))
  {
      return;
  }

  get_mac(x, s->s_name);
}

void *ether_new(void)
{
  t_ether *x = (t_ether *)pd_new(ether_class);

  x->x_symout = outlet_new(&x->x_obj, &s_symbol);

  return (void *)x;
}

void ether_setup(void)
{
  ether_class = class_new(gensym("ether"),
    (t_newmethod)ether_new, NULL,
    sizeof(t_ether), CLASS_DEFAULT, 0);

  class_addanything(ether_class, ether_doanything);
}
