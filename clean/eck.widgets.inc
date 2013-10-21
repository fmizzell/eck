<?php
/*******************************************************************************
 *            Property widget info and hooks
 ******************************************************************************/

/**
 * Returns an array of widget type options for an ECK property type.
 *
 * If no property type is provided, returns a nested array of all widget types,
 * keyed by property type.
 */
function eck_property_widget_type_options($property_type = NULL, $by_label = FALSE) {
  $options = &drupal_static(__FUNCTION__);

  if (!isset($options)) {
    $options = array();
    $property_types = eck_get_property_types();
    foreach (eck_property_info_widget_types() as $name => $widget_type) {
      foreach ($widget_type['property types'] as $widget_property_type) {
        // Check that the field type exists.
        if (isset($property_types[$widget_property_type])) {
          $options[$widget_property_type][$name] = $widget_type['label'];
        }
      }
    }
  }

  if (isset($property_type)) {
    return !empty($options[$property_type]) ? $options[$property_type] : array();
  }
  if ($by_label) {
    $property_types = eck_get_property_types();
    $options_by_label = array();
    foreach ($options as $property_type => $widgets) {
      $options_by_label[$property_types[$property_type]['label']] = $widgets;
    }
    return $options_by_label;
  }
  return $options;
}

/*******************************************************************************
 *            Form Alters
 ******************************************************************************/

/**
 * Adds a manage properties section to the field overview 'Manage fields' form
 * that allows for the inclusion of properties as "extra fields" on entity forms.
 */
function eck_form_field_ui_field_overview_form_alter(&$form, &$form_state) {
  if (array_key_exists($form['#entity_type'], eck_entity_info())) {
    $entity_type = entity_type_load($form['#entity_type']);
    $bundle = bundle_load($form['#entity_type'], $form['#bundle']);
    $admin_path = _field_ui_bundle_admin_path($bundle->entity_type, $bundle->name);
    $max_weight = field_info_max_weight($form['#entity_type'], $form['#bundle'], 'form');

    $widget_types = eck_property_info_widget_types();
    $widget_type_options = eck_property_widget_type_options(NULL, TRUE);

    $properties = array();
    $extra_fields = (!empty($bundle->config['extra_fields']) && is_array($bundle->config['extra_fields'])) ? $bundle->config['extra_fields'] : array();
    foreach ($entity_type->properties as $property => $info) {
      if (empty($extra_fields[$property]['form']))
      {
        $properties[$property] = $info;
      } else {
        $admin_property_path = $admin_path . '/properties/' . $property;
        unset($form['fields'][$property]['type']['#cell_attributes']);
        unset($form['fields'][$property]['edit']);
        unset($form['fields'][$property]['delete']);
        $table_elements = array(
          'widget_type' => array(
            '#type' => 'link',
            '#title' => t($widget_types[$extra_fields[$property]['form']['widget']['type']]['label']),
            '#href' => $admin_property_path . '/widget-type',
            '#options' => array('attributes' => array('title' => t('Change widget type.'))),
          ),
          'edit' => array(
            '#type' => 'link',
            '#title' => t('edit'),
            '#href' => $admin_property_path,
            '#options' => array('attributes' => array('title' => t('Edit property management settings.'))),
          ),
          'delete' => array(
            '#type' => 'link',
            '#title' => t('remove'),
            '#href' => $admin_property_path . '/remove',
            '#options' => array('attributes' => array('title' => t('Remove management of this property.'))),
          ),
        );
        $form['fields'][$property] += $table_elements;
      }
    }

    // Additional row: add properties as extra fields.
    if ($properties && $widget_type_options) {
      // Build list of options.
      $properties_options = array();
      foreach ($properties as $property => $info) {
        $text = t('@type: @label (@property)', array(
          '@type' => $info['type'],
          '@label' => $info['label'],
          '@property' => $property,
        ));
        $properties_options[$property] = truncate_utf8($text, 80, FALSE, TRUE);
      }
      asort($properties_options);
      $name = '_eck_add_extra_field';
      $form['fields'][$name] = array(
        '#attributes' => array('class' => array('draggable', 'tabledrag-leaf', 'add-new')),
        '#row_type' => 'add_new_field',
        '#region_callback' => 'field_ui_field_overview_row_region',
        'label' => array(
          '#type' => 'textfield',
          '#title' => t('Extra field label'),
          '#title_display' => 'invisible',
          '#size' => 15,
          '#description' => t('Label'),
          '#attributes' => array('class' => array('label-textfield')),
          '#prefix' => '<div class="label-input"><div class="add-new-placeholder">' . t('Manage a property') .'</div>',
          '#suffix' => '</div>',
        ),
        'weight' => array(
          '#type' => 'textfield',
          '#default_value' => $max_weight + 3,
          '#size' => 3,
          '#title_display' => 'invisible',
          '#title' => t('Weight for added field'),
          '#attributes' => array('class' => array('field-weight')),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
        'parent_wrapper' => array(
          'parent' => array(
            '#type' => 'select',
            '#title' => t('Parent for extra field'),
            '#title_display' => 'invisible',
            '#options' => $form['fields']['#parent_options'],
            '#empty_value' => '',
            '#attributes' => array('class' => array('field-parent')),
            '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
            '#parents' => array('fields', $name, 'parent'),
          ),
          'hidden_name' => array(
            '#type' => 'hidden',
            '#default_value' => $name,
            '#attributes' => array('class' => array('field-name')),
          ),
        ),
        'field_name' => array(
          '#type' => 'select',
          '#title' => t('Extra field'),
          '#title_display' => 'invisible',
          '#options' => $properties_options,
          '#empty_option' => t('- Select a property -'),
          '#description' => t('Existing entity property to manage.'),
          '#attributes' => array('class' => array('eck-property-type-select')),
          '#cell_attributes' => array('colspan' => 2),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
        'widget_type' => array(
          '#type' => 'select',
          '#title' => t('Widget for managed property'),
          '#title_display' => 'invisible',
          '#options' => $widget_type_options,
          '#empty_option' => t('- Select a widget -'),
          '#description' => t('Form element to edit the property data.'),
          '#attributes' => array('class' => array('eck-widget-type-select')),
          '#cell_attributes' => array('colspan' => 3),
          '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
        ),
      );
      // Include ECK specific validation and submit handling for this form.
      $form['#validate'][] = 'eck_form_field_ui_field_overview_form_validate';
      $form['#submit'][] = 'eck_form_field_ui_field_overview_form_submit';
      // Attach handling for property selection.
      $form['#attached']['js'][] = drupal_get_path('module', 'eck') . '/eck.field_ui.js';
      // Add settings for the update selects behavior.
      $js_properties = array();
      foreach ($properties as $property => $info) {
        $js_properties[$property] = array('label' => $info['label'], 'type' => $info['type']);
      }
      $form['#attached']['js'][] = array(
        'type' => 'setting',
        'data' => array('eckProperties' => $js_properties, 'eckPropertyWidgetTypes' => eck_property_widget_type_options()),
      );
    }
  }
}

/**
 * Validates the 'add property as extra field' row of field_ui_field_overview_form().
 *
 * @see eck_form_field_ui_field_overview_form_alter()
 * @see field_ui_field_overview_form_validate()
 */
function eck_form_field_ui_field_overview_form_validate($form, &$form_state) {
  // The form element might be absent if no existing fields can be added to
  // this bundle.
  if (isset($form_state['values']['fields']['_eck_add_extra_field'])) {
    $extra_field = $form_state['values']['fields']['_eck_add_extra_field'];

    // Validate if any information was provided in the 'add existing field' row.
    if (array_filter(array($extra_field['label'], $extra_field['field_name'], $extra_field['widget_type']))) {
      // Missing label.
      if (!$extra_field['label']) {
        form_set_error('fields][_eck_add_extra_field][label', t('Add property as extra field: you need to provide a label.'));
      }

      // Missing existing field name.
      if (!$extra_field['field_name']) {
        form_set_error('fields][_eck_add_extra_field][field_name', t('Add property as extra field: you need to select a field.'));
      }

      // Wrong property for this bundle.
      $entity_type = entity_type_load($form['#entity_type']);
      if (!isset($entity_type->properties[$extra_field['field_name']])) {
        form_set_error('fields][_eck_add_extra_field][field_name', t('Add property as extra field: invalid property for this bundle.'));
      }

      // Missing widget type.
      if (!$extra_field['widget_type']) {
        form_set_error('fields][_eck_add_extra_field][widget_type', t('Add property as extra field: you need to select a widget.'));
      }
      // Wrong widget type.
      elseif ($extra_field['field_name'] && ($existing_field = field_info_field($extra_field['field_name']))) {
        $entity_type = entity_type_load($form['#entity_type']);
        $widget_types = eck_property_widget_type_options($entity_type->properties[$extra_field['field_name']]['type']);
        if (!isset($widget_types[$extra_field['widget_type']])) {
          form_set_error('fields][_eck_add_extra_field][widget_type', t('Add property as extra field: invalid widget.'));
        }
      }
    }
  }
}

/**
 * Form submission handler for ECK's additions to field_ui_field_overview_form().
 *
 * @see eck_form_field_ui_field_overview_form_alter()
 * @see eck_form_field_ui_field_overview_form_validate()
 */
function eck_form_field_ui_field_overview_form_submit($form, &$form_state) {
  if (isset($form_state['values']['fields']['_eck_add_extra_field'])) {
    $extra_field_settings = $form_state['values']['fields']['_eck_add_extra_field'];
    // Submit only if information was provided in the 'add existing field' row.
    if (array_filter(array($extra_field_settings['label'], $extra_field_settings['field_name'], $extra_field_settings['widget_type']))) {
      $entity_type = entity_type_load($form['#entity_type']);
      $bundle = bundle_load($form['#entity_type'], $form['#bundle']);
      $property_name = $extra_field_settings['field_name'];

      // Add extra field configuration for this property to the bundle.
      $config = (!empty($bundle->config)) ? $bundle->config : array();
      $widget_info = eck_property_info_widget_types($extra_field_settings['widget_type']);
      $extra_field = array(
        'label' => $extra_field_settings['label'],
        'description' => $widget_info['description'],
        'widget' => array(
          'type' => $extra_field_settings['widget_type'],
          'settings' => $widget_info['settings'],
        ),
      );
      // If the property behavior supplies a default value function then make sure we use it.
      if($default_function = eck_property_behavior_implements($entity_type, $property_name, 'default_value')){
        $extra_field['default_value_function'] = $default_function;
      }
      // Pull in the default..default value from the property type schema.
      $schema = eck_get_property_type_schema($entity_type->properties[$property_name]['type']);
      $extra_field['default_value'] = $schema['default'];
      $config['extra_fields'][$property_name]['form'] = $extra_field;
      $bundle->config = $config;
      // Save the bundle.
      $bundle->save();
      Bundle::loadAll(NULL, TRUE);
    }
  }
}

/**
 * Implements hook_modules_enabled().
 */
function eck_modules_enabled($modules) {
  $reset_caches = FALSE;
  foreach ($modules as $module) {
    if (module_hook($module, 'eck_property_widget_info') || module_hook($module, 'eck_property_types')) {
      eck_flush_caches();
      break;
    }
  }
}

/**
 * Implements hook_modules_disabled().
 */
function eck_modules_disabled($modules) {
  $reset_caches = FALSE;
  foreach ($modules as $module) {
    if (module_hook($module, 'eck_property_widget_info') || module_hook($module, 'eck_property_types')) {
      eck_flush_caches();
      break;
    }
  }
}
