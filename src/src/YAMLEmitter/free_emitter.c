#include <yaml.h>

#include "sac.h"

void free_emitter( yaml_emitter_t * emitter)
{
  yaml_emitter_delete(emitter);
  SAC_FREE( emitter);
}
