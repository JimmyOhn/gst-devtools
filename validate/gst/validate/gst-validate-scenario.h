/* GStreamer
 * Copyright (C) 2013 Thibault Saunier <thibault.saunier@collabora.com>
 *
 * gst-validate-runner.c - Validate Runner class
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_VALIDATE_SCENARIO_H__
#define __GST_VALIDATE_SCENARIO_H__

#include <glib.h>
#include <glib-object.h>

#include <gst/validate/gst-validate-runner.h>

G_BEGIN_DECLS

#define GST_TYPE_VALIDATE_SCENARIO            (gst_validate_scenario_get_type ())
#define GST_VALIDATE_SCENARIO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenario))
#define GST_VALIDATE_SCENARIO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenarioClass))
#define GST_IS_VALIDATE_SCENARIO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_SCENARIO))
#define GST_IS_VALIDATE_SCENARIO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_VALIDATE_SCENARIO))
#define GST_VALIDATE_SCENARIO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenarioClass))

typedef struct _GstValidateScenario        GstValidateScenario;
typedef struct _GstValidateScenarioClass   GstValidateScenarioClass;
typedef struct _GstValidateScenarioPrivate GstValidateScenarioPrivate;
typedef struct _GstValidateAction          GstValidateAction;
typedef struct _GstValidateActionParameter GstValidateActionParameter;

GST_EXPORT GType _gst_validate_action_type;

enum
{
  GST_VALIDATE_EXECUTE_ACTION_ERROR,
  GST_VALIDATE_EXECUTE_ACTION_OK,
  GST_VALIDATE_EXECUTE_ACTION_ASYNC
};

/* TODO 2.0 -- Make it an actual enum type */
#define GstValidateExecuteActionReturn gint

/**
 * GstValidateExecuteAction:
 * @scenario: The #GstValidateScenario from which the @action is executed
 * @action: The #GstValidateAction being executed
 *
 * A function that executes a #GstValidateAction
 *
 * Returns: a #GstValidateExecuteActionReturn
 */
typedef GstValidateExecuteActionReturn (*GstValidateExecuteAction) (GstValidateScenario * scenario, GstValidateAction * action);


/**
 * GstValidateAction:
 * @type: The type of the #GstValidateAction, which is the name of the
 *        GstValidateActionType registered with
 *        #gst_validate_register_action_type
 * @name: The name of the action, set from the user in the scenario
 * @structure: the #GstStructure defining the action
 *
 * The GstValidateAction defined to be executed as part of a scenario
 */
struct _GstValidateAction
{
  GstMiniObject          mini_object;

  /*< public > */
  const gchar *type;
  const gchar *name;
  GstStructure *structure;
  GstValidateScenario *scenario;

  /* < private > */
  guint action_number;
  gint repeat;
  GstClockTime playback_time;
  GstValidateExecuteActionReturn state; /* Actually ActionState */

  gpointer _gst_reserved[GST_PADDING_LARGE - sizeof (gint) - 1];
};

void gst_validate_action_set_done (GstValidateAction *action);

#define GST_TYPE_VALIDATE_ACTION            (gst_validate_action_get_type ())
#define GST_IS_VALIDATE_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_ACTION))
GType gst_validate_action_get_type (void);

typedef struct _GstValidateActionType      GstValidateActionType;

/**
 * GstValidateActionTypeFlags:
 * @GST_VALIDATE_ACTION_TYPE_NONE: No special flag
 * @GST_VALIDATE_ACTION_TYPE_CONFIG: The action is a config
 * @GST_VALIDATE_ACTION_TYPE_ASYNC: The action can be executed ASYNC
 */
typedef enum
{
    GST_VALIDATE_ACTION_TYPE_NONE = 0,
    GST_VALIDATE_ACTION_TYPE_CONFIG = 1 << 1,
    GST_VALIDATE_ACTION_TYPE_ASYNC = 1 << 2,
} GstValidateActionTypeFlags;

struct _GstValidateActionType
{
  GstMiniObject          mini_object;

  gchar *name;
  gchar *implementer_namespace;

  GstValidateExecuteAction execute;

  GstValidateActionParameter *parameters;

  gchar *description;
  GstValidateActionTypeFlags flags;

  gpointer _gst_reserved[GST_PADDING_LARGE];
};

#define GST_TYPE_VALIDATE_ACTION_TYPE       (gst_validate_action_type_get_type ())
#define GST_IS_VALIDATE_ACTION_TYPE(obj)    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_ACTION_TYPE))
#define GST_VALIDATE_ACTION_TYPE(obj)       ((GstValidateActionType*) obj)
GType gst_validate_action_type_get_type     (void);

gboolean gst_validate_print_action_types (const gchar ** wanted_types, gint num_wanted_types);

/**
 * GstValidateActionParameter:
 * @name: The name of the parameter
 * @description: The description of the parameter
 * @mandatory: Whether the parameter is mandatory for
 *             a specific action type
 * @types: The types the parameter can take described as a
 * string. It can be precisely describing how the typing works
 *         using '\n' between the various acceptable types.
 * @possible_variables: The name of the variables that can be
 *                      used to compute the value of the parameter.
 *                      For example for the start value of a seek
 *                      action, we will accept to take 'duration'
 *                      which will be replace by the total duration
 *                      of the stream on which the action is executed.
 * @def: The default value of a parametter as a string, should be %NULL
 *       for mandatory streams.
 */
struct _GstValidateActionParameter
{
  const gchar  *name;
  const gchar  *description;
  gboolean     mandatory;
  const gchar  *types;
  const gchar  *possible_variables;
  const gchar  *def;

  /*< private >*/
  gpointer     _gst_reserved[GST_PADDING];
};

struct _GstValidateScenarioClass
{
  GObjectClass parent_class;

  /*< public >*/
  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GstValidateScenario:
 * @pipeline: The #GstPipeline on which the scenario is being executed.
 */
struct _GstValidateScenario
{
  GObject parent;

  /*< public >*/
  GstElement *pipeline;

  /*< private >*/
  GstValidateScenarioPrivate *priv;

  gpointer _gst_reserved[GST_PADDING];
};

GType gst_validate_scenario_get_type (void);

GstValidateScenario * gst_validate_scenario_factory_create (GstValidateRunner *runner,
                                                GstElement *pipeline,
                                                const gchar *scenario_name);
gboolean
gst_validate_list_scenarios       (gchar **scenarios,
                                   gint num_scenarios,
                                   gchar * output_file);

GstValidateActionType *
gst_validate_get_action_type           (const gchar *type_name);
void gst_validate_register_action_type (const gchar *type_name,
                                        const gchar *implementer_namespace,
                                        GstValidateExecuteAction function,
                                        GstValidateActionParameter * parameters,
                                        const gchar *description,
                                        GstValidateActionTypeFlags flags);


gboolean gst_validate_action_get_clocktime (GstValidateScenario * scenario,
                                            GstValidateAction *action,
                                            const gchar * name,
                                            GstClockTime * retval);

gboolean gst_validate_scenario_execute_seek (GstValidateScenario *scenario,
                                             GstValidateAction *action,
                                             gdouble rate,
                                             GstFormat format,
                                             GstSeekFlags flags,
                                             GstSeekType start_type,
                                             GstClockTime start,
                                             GstSeekType stop_type,
                                             GstClockTime stop);

G_END_DECLS

#endif /* __GST_VALIDATE_SCENARIOS__ */
