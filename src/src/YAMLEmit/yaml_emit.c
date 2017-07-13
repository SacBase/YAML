#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <yaml.h>

#include "sac.h"

// This might change as libyaml gets upgrade to YAML 1.2...
static const char * yaml_event_name[] = {
  "NULL Event", "Stream Start", "Stream End", "Document Start",
  "Document End", "Alias Event", "Scalar Event", "Sequence Start",
  "Sequence End", "Mapping Start", "Mapping End"
};

yaml_emitter_t * yaml_create_emitter( FILE *stream)
{
  yaml_emitter_t * emitter;
  emitter = SAC_MALLOC( sizeof(yaml_emitter_t));

  yaml_emitter_initialize(emitter);
  yaml_emitter_set_output_file(emitter, stream);
  yaml_emitter_set_canonical(emitter, 0);
  return emitter;
}

void print_emitter_error(yaml_emitter_t * emitter)
{
  switch(emitter->error)
  {
    case YAML_MEMORY_ERROR:
      fprintf(stderr, "Memory error: Not enough memory for emitting\n");
      break;
    case YAML_WRITER_ERROR:
      fprintf(stderr, "Writer error: %s\n", emitter->problem);
      break;
    case YAML_EMITTER_ERROR:
      fprintf(stderr, "Emitter error: %s\n", emitter->problem);
      break;
    default:
      /* Couldn't happen. */
      fprintf(stderr, "Internal error\n");
      break;
  }
}

int yaml_emit(yaml_emitter_t * emitter, yaml_event_type_t event, ...)
{
  yaml_event_t event_s;
  yaml_char_t * copy, * tag;
  yaml_scalar_style_t style;
  va_list valist;
  int status = EXIT_SUCCESS;

  switch(event)
  {
    case YAML_STREAM_START_EVENT:
      status = yaml_stream_start_event_initialize(&event_s, YAML_UTF8_ENCODING);
      break;
    case YAML_STREAM_END_EVENT:
      status = yaml_stream_end_event_initialize(&event_s);
      break;
    case YAML_DOCUMENT_START_EVENT:
      status = yaml_document_start_event_initialize(
          &event_s, NULL, NULL, NULL, 0);
      break;
    case YAML_DOCUMENT_END_EVENT:
      status = yaml_document_end_event_initialize(
          &event_s, 1);
      break;
    case YAML_MAPPING_START_EVENT:
      status = yaml_mapping_start_event_initialize(
          &event_s, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE);
      break;
    case YAML_MAPPING_END_EVENT:
      status = yaml_mapping_end_event_initialize(&event_s);
      break;
    case YAML_SEQUENCE_START_EVENT:
      status = yaml_sequence_start_event_initialize(
          &event_s, NULL, NULL, 1, YAML_BLOCK_SEQUENCE_STYLE);
      break;
    case YAML_SEQUENCE_END_EVENT:
      status = yaml_sequence_end_event_initialize(&event_s);
      break;
    case YAML_SCALAR_EVENT:
      va_start(valist, event);
      copy = va_arg(valist, yaml_char_t *);
      style = va_arg(valist, yaml_scalar_style_t);
      tag = va_arg(valist, yaml_char_t *);
      va_end(valist);

      status = yaml_scalar_event_initialize(
          &event_s, NULL, tag, copy, -1,
          0, 0, style);
      break;
    case YAML_ALIAS_EVENT:
      va_start(valist, event);
      copy = va_arg(valist, yaml_char_t *);
      va_end(valist);

      status = yaml_alias_event_initialize(&event_s, copy);
      break;
    default:
      fprintf(stderr, "Unknown event: %d\n", event);
      status = -1;
      goto exit_error;
      break;
  }

  status = yaml_emitter_emit(emitter, &event_s);
  if(!status)
  {
    fprintf(stderr, "Emitter event: %s, ", yaml_event_name[event]);
    print_emitter_error(emitter);
  }

exit_error:

  return status;
}

int yaml_value_with_tag(yaml_emitter_t * emitter, yaml_char_t * value, yaml_scalar_style_t style, yaml_char_t * tag)
{
  return yaml_emit(emitter, YAML_SCALAR_EVENT, value, style, tag);
}

int yaml_valuef(yaml_emitter_t * emitter, yaml_scalar_style_t style, yaml_char_t * tag,  const char * format, ...)
{
  va_list valist;
  char str[1024];
  int status = EXIT_SUCCESS;

  va_start(valist, format);
  status = vsnprintf(str, sizeof(char)*1024, format, valist);
  va_end(valist);

  if(status < 0)
  {
    fprintf(stderr, "Oops vsnprintf failed for some reason\n");
    goto errors;
  } else { // we don't handle the case where we truncate
    status = yaml_value_with_tag(emitter, (yaml_char_t *) str, style, tag);
  }

errors:

  return status;
}

int yaml_valuef_str(yaml_emitter_t * emitter, const char * format, ...)
{
  va_list valist;
  char str[1024];
  int status = EXIT_SUCCESS;

  va_start(valist, format);
  status = vsnprintf(str, sizeof(char)*1024, format, valist);
  va_end(valist);

  if(status < 0)
  {
    fprintf(stderr, "Oops vsnprintf failed for some reason\n");
    goto errors;
  } else { // we don't handle the case where we truncate
    status = yaml_value_with_tag(emitter, (yaml_char_t *) str, YAML_SINGLE_QUOTED_SCALAR_STYLE, (yaml_char_t *) YAML_STR_TAG);
  }

errors:

  return status;
}

int yaml_valuef_int(yaml_emitter_t * emitter, const char * format, ...)
{
  va_list valist;
  char str[1024];
  int status = EXIT_SUCCESS;

  va_start(valist, format);
  status = vsnprintf(str, sizeof(char)*1024, format, valist);
  va_end(valist);

  if(status < 0)
  {
    fprintf(stderr, "Oops vsnprintf failed for some reason\n");
    goto errors;
  } else { // we don't handle the case where we truncate
    status = yaml_value_with_tag(emitter, (yaml_char_t *) str, YAML_PLAIN_SCALAR_STYLE, (yaml_char_t *) YAML_INT_TAG);
  }

errors:

  return status;
}

int yaml_valuef_flt(yaml_emitter_t * emitter, const char * format, ...)
{
  va_list valist;
  char str[1024];
  int status = EXIT_SUCCESS;

  va_start(valist, format);
  status = vsnprintf(str, sizeof(char)*1024, format, valist);
  va_end(valist);

  if(status < 0)
  {
    fprintf(stderr, "Oops vsnprintf failed for some reason\n");
    goto errors;
  } else { // we don't handle the case where we truncate
    status = yaml_value_with_tag(emitter, (yaml_char_t *) str, YAML_PLAIN_SCALAR_STYLE, (yaml_char_t *) YAML_FLOAT_TAG);
  }

errors:

  return status;
}

int yaml_value_str(yaml_emitter_t * emitter, yaml_char_t * value)
{
  return yaml_value_with_tag(emitter, value, YAML_SINGLE_QUOTED_SCALAR_STYLE, (yaml_char_t *) YAML_STR_TAG);
}

int yaml_value_bool(yaml_emitter_t * emitter, yaml_char_t * value)
{
  return yaml_value_with_tag(emitter, value, YAML_PLAIN_SCALAR_STYLE, (yaml_char_t *) YAML_BOOL_TAG);
}

int yaml_value_int(yaml_emitter_t * emitter, const int value)
{
  return yaml_valuef_int(emitter, "%d", value);
}

int yaml_value_float(yaml_emitter_t * emitter, const float value)
{
  return yaml_valuef_flt(emitter, "%f", value);
}


int yaml_map_begin(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_MAPPING_START_EVENT);
}

int yaml_map_end(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_MAPPING_END_EVENT);
}

int yaml_seq_begin(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_SEQUENCE_START_EVENT);
}

int yaml_seq_end(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_SEQUENCE_END_EVENT);
}

int yaml_stream_begin(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_STREAM_START_EVENT);
}

int yaml_stream_end(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_STREAM_END_EVENT);
}

int yaml_doc_begin(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_DOCUMENT_START_EVENT);
}

int yaml_doc_end(yaml_emitter_t * emitter)
{
  return yaml_emit(emitter, YAML_DOCUMENT_END_EVENT);
}

// vim: ts=2 sw=2 et:
