<?php
/**
 * @file
 * All of the menu, pages, and forms related to bundle administration.
 */

module_load_include('inc', 'eck', 'eck.classes');

/**
 * This function creates the menu items relevant to bundle administration.
 *
 * @param /EntityType $entity_type
 *   As returned by eck__entity_type__load().
 *
 * @see eck_menu()
 */
function eck__bundle__menu($entity_type) {
  // Create the menus relavant to types.
  $path = eck__entity_type__path();
  $menu = array();

  // EDIT an entity type.
  $menu["{$path}/{$entity_type->name}/edit"] = array(
    'title' => "Edit",
    'description' => "Edit the '{$entity_type->label}' entity type",
    'page callback' => 'drupal_get_form',
    'page arguments' => array("eck__entity_type__form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'entity_type', $entity_type),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
  );

  // DELETE an entity type.
  $menu["{$path}/{$entity_type->name}/delete"] = array(
    'title' => "Delete",
    'description' => "Delete the '{$entity_type->label}' Entity Type",
    // "eck__entity_type__delete",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__entity_type__delete_form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("delete", "entity_type", $entity_type),
    'file' => 'eck.entity_type.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 3,
  );

  // MANAGE Entity Type Properties.
  $menu["{$path}/{$entity_type->name}/property"] = array(
    'title' => "Manage properties",
    'description' => "Manage the properties of the {$entity_type->label} entity type",
    'page callback' => "eck__properties",
    'page arguments' => array($entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 4,
  );

  // Add a property.
  $menu["{$path}/{$entity_type->name}/property/add"] = array(
    'title' => "Add property",
    'description' => "Add a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__property_add__form", $entity_type->name),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
    'type' => MENU_LOCAL_ACTION,
    'weight' => 4,
  );

  // Activate a default property.
  $menu["{$path}/{$entity_type->name}/property/%/activate"] = array(
    'title' => "Activate default property",
    'page callback' => "eck__activate_default_property",
    'page arguments' => array($entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.default_properties.inc',
  );

  // Dectivate a default property.
  $menu["{$path}/{$entity_type->name}/property/%/deactivate"] = array(
    'title' => "Activate default property",
    'page callback' => "eck__deactivate_default_property",
    'page arguments' => array($entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.default_properties.inc',
  );

  // Edit a property.
  $menu["{$path}/{$entity_type->name}/property/%/edit"] = array(
    'title' => "Edit property",
    'description' => "Edit a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array("eck__property_edit__form", $entity_type->name, 5),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );

  // Delete a property.
  $menu["{$path}/{$entity_type->name}/property/%/delete"] = array(
    'title' => "Delete property",
    'description' => "Delete a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array(
      "eck__property_delete__form",
      $entity_type->name,
      5,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );

  // Modify (add/change/delete) the behavior on a property.
  // Edit a property.
  $menu["{$path}/{$entity_type->name}/property/%/behavior"] = array(
    'title' => "Edit property",
    'description' => "Modify the behavior of a property of the {$entity_type->label} entity type",
    'page callback' => "drupal_get_form",
    'page arguments' => array(
      "eck__property_behavior__form",
      $entity_type->name,
      5,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array("update", "property"),
    'file' => 'eck.properties.inc',
  );

  // LIST bundles of an entity type.
  $menu["{$path}/{$entity_type->name}"] = array(
    'title' => "{$entity_type->label}",
    'description' => "View all the bundles for '{$entity_type->label}'",
    'page callback' => "eck__bundle__list",
    'page arguments' => array(3),
    'access callback' => 'eck_access',
    'access arguments' => array('list', 'bundle', $entity_type),
    'file' => 'eck.bundle.inc',
  );

  $menu["{$path}/{$entity_type->name}/list"] = array(
    'title' => 'Bundle List',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100,
  );

  // ADD bundles to an entity type.
  $menu["{$path}/{$entity_type->name}/add"] = array(
    'title' => "Add bundle ",
    'description' => "Add a(n) new '{$entity_type->label} Bundle'",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__add', 3),
    'access callback' => 'eck_access',
    'access arguments' => array('create', 'bundle', $entity_type),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 0,
    'file' => 'eck.bundle.inc',
  );

  module_load_include('inc', 'eck', 'eck.entity');
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
    $menu = array_merge($menu, eck__entity__menu($entity_type, $bundle));
  }

  return $menu;
}

/**
 * Page call back for the bundle overview table.
 *
 * to see and manipulate all created label of a given type.
 *
 * @param string $entity_type
 *   The name of the entity type.
 */
function eck__bundle__list($entity_type) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type);

  // Check that the user has permissions to view bundle lists.
  if (eck_access('list', 'bundle') || eck_access('list', "bundle", $entity_type)) {

    $header = array(
      t('Type'),
      array('data' => t('Operations'), 'colspan' => '1'),
    );

    $rows = array();

    $bundles = Bundle::loadByEntityType($entity_type);
    usort($bundles, 'eck_alphabetical_cmp');

    foreach ($bundles as $bundle) {
      $bundle_label = $bundle->label;

      $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);

      $uri = $admin_info['path'];

      $allowed_operations = '';
      // Check that the user has permissions to delete.
      if (eck_access('delete', 'bundle', $bundle)) {
        $allowed_operations = l(t('delete'), $uri . "/delete");
      }

      if (eck_access('list', 'entity') || eck_access('list', 'entity', $bundle)) {
        $rows[] = array(
          l($bundle_label, url($uri, array('absolute' => TRUE))),
          $allowed_operations,
        );
      }
      else {
        $rows[] = array($bundle_label, $allowed_operations);
      }
    }

    $build['bundle_table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
  }

  return $build;
}

/**
 * Bundle add from callback.
 */
function eck__bundle__add($form, &$form_state, $entity_type) {
  $entity_type = entity_type_load($entity_type);
  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Type'),
    '#description' => "A Human readable name for the bundle",
  );

  $form['bundle_name'] = array(
    '#type' => 'machine_name',
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('bundle_label'),
    ),
  );

  $form['#validate'][] = 'eck__bundle__add_validate';

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Bundle add form validation callback.
 */
function eck__bundle__add_validate($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];

  // The type does not have to be unique in the table, but it should be unique
  // to its entity so we will check that here.
  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {

    if ($bundle->name == $form_state['values']['bundle_name']) {
      form_set_error('bundle', t("bundle @bundle_name already exists for this entity type",
        array("@bundle_name" => $bundle->name)));
    }
  }
}

/**
 * Add bundle form callback.
 */
function eck__bundle__add_submit($form, &$form_state) {
  $path = eck__entity_type__path();

  $entity_type = $form_state['values']['entity_type'];
  $bundle_name = $form_state['values']['bundle_name'];
  $bundle_label = $form_state['values']['bundle_label'];

  $bundle = new Bundle();
  $bundle->name = $bundle_name;
  $bundle->label = $bundle_label;
  $bundle->entity_type = $entity_type->name;
  $bundle->save();

  drupal_set_message(t('the bundle %bundle for entity type %entity_type has been created.',
    array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));

  Bundle::loadAll(NULL, TRUE);
  drupal_get_schema(NULL, TRUE);
  entity_info_cache_clear();
  variable_set('menu_rebuild_needed', TRUE);

  $form_state['redirect'] = "{$path}/{$entity_type->name}";
}

/**
 * Form constructor for the entity bundle editing form.
 *
 * @param string $entity_type_name
 *   Entity type name.
 * @param string $bundle_name
 *   Entity bundle name.
 */
function eck__bundle__edit_form($form, &$form_state, $entity_type_name, $bundle_name) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  $form = array();

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  // Let the behaviors to modify form.
  $vars = eck_property_behavior_invoke_plugin_alter($entity_type, 'bundle_form', array('form' => $form, 'form_state' => $form_state));
  $form = $vars['form'];
  $form_state = $vars['form_state'];

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save bundle'),
    '#weight' => 40,
  );
  return $form;
}

/**
 * Budle edit form submit callback.
 */
function eck__bundle__edit_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  // Save all form values starting from 'config_' to the $bundle->config array.
  $bundle_config = array();
  foreach ($form_state['values'] as $name => $value) {
    if (strpos($name, 'config_') === 0) {
      // We don't need 'config_' prefix.
      $bundle_config[substr($name, 7)] = $form_state['values'][$name];
    }
  }
  $bundle->config = $bundle_config;
  // Save the bundle object to the database.
  $bundle->save();

  drupal_set_message(t('the bundle %bundle for entity type %entity_type has been updated.',
    array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));
}

/**
 * Delete the bundle of a given entity type.
 *
 * @param \EntityType $entity_type
 *   The entity type of the bundle that will be deleted.
 * @param \Bundle $bundle
 *   The bundle to be deleted.
 *
 * @see eck.classes.inc
 */
function eck__bundle__delete($entity_type, $bundle) {
  // And finally we delete the bundle from the eck_type table.
  $bundle->delete();

  drupal_get_schema(NULL, TRUE);
  variable_set('menu_rebuild_needed', TRUE);
  drupal_flush_all_caches();

  drupal_set_message(t('The bundle %bundle from the entity type %entity_type has been deleted.',
    array('%bundle' => $bundle->label, '%entity_type' => $entity_type->label)));
}

/**
 * Bundle delete form callback.
 */
function eck__bundle__delete_form($form, &$form_state, $entity_type_name, $bundle_name) {
  $path = eck__entity_type__path();
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  $form['submit_redirect'] = array(
    '#type' => 'value',
    '#value' => "{$path}/{$entity_type->name}",
  );

  $message = t('Are you sure that you want to delete the bundle %bundle?',
    array('%bundle' => $bundle->label));

  $caption = t("All of the data (entities) associated with this bundle
  will be deleted. This action cannot be undone.");

  return confirm_form($form, $message, "{$path}/{$entity_type->name}", $caption, t('Delete'));
}

/**
 * Submit form callback to delete bundles.
 */
function eck__bundle__delete_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  eck__bundle__delete($entity_type, $bundle);

  // Ok, lets delete the entity.
  $form_state['redirect'] = $submit_redirect;
}

/**
 * Autocomplete functionality for entities using field labels.
 */
function eck__bundle__field_autocomplete($entity_type, $bundle, $string = "") {
  $field_label = get_bundle_field_label_info($entity_type, $bundle);
  $matches = array();

  if ($field_label) {
    $field = $field_label['field'];
    $language = $field_label['language'];
    $delta = $field_label['delta'];
    $column = $field_label['column'];
    $query = new EntityFieldQuery();
    // @todo Remove the addmetadata() call below once
    // http://drupal.org/node/997394 is fixed.
    $query->addMetadata('account', user_load(1));

    $query->entityCondition('entity_type', $entity_type->name, '=')->entityCondition('bundle', $bundle->name, '=')->fieldCondition($field, $column, $string, 'CONTAINS');

    $results = $query->execute();

    $entities = entity_load($entity_type->name, array_keys($results[$entity_type->name]));
    foreach ($entities as $id => $entity) {
      $matches[$id] = $entity->{$field}[$language][$delta][$column];
    }
  }

  drupal_json_output($matches);
}

/**
 * Helper function to determine if an entity wants to define a field as lable.
 */
function get_bundle_field_label_info($entity_type, $bundle) {

  $info = entity_get_info();

  if (array_key_exists('field label', $info[$entity_type->name]['bundles'][$bundle->name])) {
    return $info[$entity_type->name]['bundles'][$bundle->name]['field label'];
  }
  else {
    return NULL;
  }
}

/**
 ********** Properties as extra fields ***********
 */

/**
 * Form for managing the extra field settings for a given entity property.
 *
 * @param array $form
 *   The entity property extra field management form. For configuration of extra
 *   field settings like its label.
 * @param array $form_state
 *   The current state of the form.
 * @param string $entity_type_name
 *   The machine name of the entity type.
 * @param string $bundle_type
 *   The machine name of the bundle type.
 * @param string $property_name
 *   The machine name of the entity property being included as an extra field.
 *
 * @return array
 *   An array comprising the form for managing the settings for the extra field.
 */
function eck__manage_extra_field_form($form, &$form_state, $entity_type_name, $bundle_type, $property_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_type);
  drupal_set_title($bundle->config['extra_fields'][$property_name]['form']['label']);

  if (empty($bundle->config['extra_fields'][$property_name]['form'])) {
    $form_state['redirect'] = 'admin/structure/entity-type/' . $entity_type_name . '/' . $bundle->name . '/fields';
    return array();
  }
  $bundle_property_config = $bundle->config['extra_fields'][$property_name]['form'];
  $widget_type = eck_property_info_widget_types($bundle_property_config['widget']['type']);

  $form = array(
    '#entity_type' => $entity_type,
    '#bundle' => $bundle,
    '#property_name' => $property_name,
  );
  // Build the configurable property extra field form values.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => !empty($bundle_property_config['label']) ? $bundle_property_config['label'] : $property_name,
    '#required' => TRUE,
    '#weight' => -20,
  );
  $form['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Required field'),
    '#default_value' => !empty($bundle_property_config['required']),
    '#weight' => -10,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Help text'),
    '#default_value' => !empty($bundle_property_config['description']) ? $bundle_property_config['description'] : '',
    '#rows' => 5,
    '#description' => t('Instructions to present to the user below this property field on the editing form.<br />Allowed HTML tags: @tags', array('@tags' => _field_filter_xss_display_allowed_tags())),
    '#weight' => -5,
  );

  // Include external module file dependency if one is required.
  if (function_exists('drupal_get_path') && $widget_type['file']) {
    form_load_include($form_state, $widget_type['file type'], $widget_type['module'], $widget_type['file']);
  }

  // Add additional widget settings from the property's widget type module.
  $additions = module_invoke($widget_type['module'], 'eck_property_widget_settings_form', $entity_type, $bundle, $property_name, $bundle_property_config);
  if (is_array($additions)) {
    $form['widget'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('%type widget settings', array('%type' => $widget_type['label'])),
    );
    $form['widget']['settings'] = $additions;
  }

  // Add handling for default value if not provided by any other module.
  if (empty($bundle_property_config['default_value_function'])) {
    $function = $widget_type['module'] . '_eck_property_widget_form';
    if (function_exists($function)) {
      $form['default_value_widget'] = array(
        '#type' => 'fieldset',
        '#title' => t('Default value'),
        '#collapsible' => FALSE,
        '#tree' => TRUE,
        '#description' => t('The default value for this property field, used when creating new content.'),
        // Stick to an empty 'parents' on this form in order not to breaks
        // widgets that do not use field_widget_[field|instance]() and still
        // access $form_state['field'] directly.
        '#parents' => array(),
      );
      $element = array(
        '#entity' => NULL,
        '#entity_type' => $entity_type,
        '#bundle' => $bundle,
        '#property_name' => $property_name,
        '#widget_type' => $widget_type,
        '#language' => LANGUAGE_NONE,
        '#title' => check_plain($bundle_property_config['label']),
        '#description' => field_filter_xss($bundle_property_config['description']),
        '#required' => (!empty($bundle_property_config['required'])),
      );
      // Populate widgets with default values when creating a new entity.
      if ($element = $function($form, $form_state, $property_name, $bundle_property_config, LANGUAGE_NONE, $bundle_property_config['default_value'], $element)) {
        // Allow modules to alter the property widget form element.
        $context = array(
          'form' => $form,
          'property_name' => $property_name,
          'bundle_property_config' => $bundle->config['extra_fields'][$property_name],
          'langcode' => LANGUAGE_NONE,
          'value' => $bundle_property_config['default_value'],
        );
        drupal_alter(array('eck_property_widget_form', 'eck_property_widget_' . $bundle_property_config['widget']['type'] . '_form'), $element, $form_state, $context);
      }
      $form['default_value_widget'][$property_name] = $element;
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Continue'),
  );
  return $form;
}

/**
 * Submit callback.
 *
 * For editing a property field's settings from a bundle's field management
 * form.
 */
function eck__manage_extra_field_form_submit($form, &$form_state) {
  // Load entity_type and bundle in case they have changed since the form was
  // first built.
  $entity_type = entity_type_load($form['#entity_type']->name);
  $bundle = bundle_load($entity_type->name, $form['#bundle']->name);
  // Update the property field settings.
  if (isset($bundle->config['extra_fields'][$form['#property_name']]['form'])) {
    $property_settings = $bundle->config['extra_fields'][$form['#property_name']];
    $widget_info = eck_property_info_widget_types($property_settings['form']['widget']['type']);
    $property_settings['form']['required'] = !(empty($form_state['values']['required']));
    $property_settings['form']['label'] = $form_state['values']['label'];
    $property_settings['form']['description'] = $form_state['values']['description'];

    // Update the widget settings if there are any.
    if (isset($form['widget']['settings'])) {
      $property_settings['form']['widget']['settings'] = $form_state['values']['widget']['settings'];
    }

    // Handle the default value.
    if (isset($form['default_value_widget'])) {
      $property_settings['form']['default_value'] = eck_property_widget_extract_value($entity_type, $bundle, $form['#property_name'], $widget_info, $form, $form_state);
    }

    // Now update the bundle config.
    $config = $bundle->config;
    $config['extra_fields'][$form['#property_name']] = $property_settings;
    $bundle->config = $config;
    // Save the bundle.
    $bundle->save();
    Bundle::loadAll(NULL, TRUE);
  }
  else {
    // The extra field form setting was not found in the config so it must no
    // longer be managed. Don't do anything except notify the user.
    drupal_set_message(t('Could not edit the property field settings. The property %name on the %bundle bundle is no selected for management.', array('%name' => $form['#property_name'], '%bundle' => $bundle->label)), 'error');
  }
  $form_state['redirect'] = 'admin/structure/entity-type/' . $entity_type->name . '/' . $bundle->name . '/fields';
}

/**
 * Helper function for extracting the value for a property from its widget form.
 *
 * If one is available the 'value callback' function for the widget is called to
 * processe the values returned.
 *
 * @param object $entity_type
 *   The ECK entity_type object.
 * @param object $bundle
 *   The ECK bundle object.
 * @param string $property_name
 *   The machine name of the property for which the value should be returned.
 * @param array $widget
 *   The widget info array as returned by eck_property_info_widget_types(TYPE).
 * @param array $form
 *   The submitted form array.
 * @param array $form_state
 *   The array containing the current state information for the form.
 *
 * @return mixed
 *   The value that should be saved for this property.
 */
function eck_property_widget_extract_value($entity_type, $bundle, $property_name, $widget, $form, &$form_state) {
  $value = NULL;
  // If the widget has a callback to extract the value then use it.
  if (!empty($widget['value callback']) && function_exists($widget['value callback'])) {
    $value = $widget['value callback']($entity_type, $bundle, $property_name, $widget, $form, $form_state);
  }
  else {
    // By default just take the value matching the property name.
    if (isset($form_state['values'][$property_name])) {
      $value = $form_state['values'][$property_name];
    }
  }
  return $value;
}

/**
 * Form for changing the widget type for a given entity property.
 *
 * @param array $form
 *   The property widget form.
 * @param array $form_state
 *   The current state of the form.
 * @param string $entity_type_name
 *   The machine name of the entity type.
 * @param string $bundle_type
 *   The machine name of the bundle type.
 * @param string $property_name
 *   The machine name of the entity property being included as an extra field.
 *
 * @return array
 *   An array comprising the form for changing the property's widget.
 */
function eck__extra_field_widget_form($form, &$form_state, $entity_type_name, $bundle_type, $property_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_type);
  drupal_set_title($bundle->config['extra_fields'][$property_name]['form']['label']);

  $default_widget = (!empty($bundle->config['extra_fields'][$property_name]['form']['widget']['type'])) ? $bundle->config['extra_fields'][$property_name]['form']['widget']['type'] : '';

  $form = array(
    '#entity_type' => $entity_type,
    '#bundle' => $bundle,
    '#property_name' => $property_name,
    '#default_widget' => $default_widget,
  );
  $form['widget'] = array(
    '#type' => 'fieldset',
    '#title' => t('Change Widget'),
    '#weight' => 5,
    '#collapsible' => FALSE,
  );

  $form['widget']['widget_type'] = array(
    '#type' => 'select',
    '#title' => t('Widget for managed property'),
    '#options' => eck_property_widget_type_options($entity_type->properties[$property_name]['type']),
    '#empty_option' => t('- Select a widget -'),
    '#description' => t('The type of form element you would like to present to the user when managing this property for the %bundle type.', array('%type' => $bundle->label)),
    '#default_value' => $default_widget,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Continue'),
  );
  return $form;
}

/**
 * Validation callback.
 *
 * To change the widget for a property field from a bundle's field management
 * form.
 */
function eck__extra_field_widget_form_validate($form, &$form_state) {
  // Missing widget type.
  if (empty($form_state['values']['widget_type'])) {
    form_set_error('widget_type', t('You need to select a widget.'));
  }
  // Wrong widget type.
  else {
    $entity_type = entity_type_load($form['#bundle']->name);
    $widget_types = eck_property_widget_type_options($entity_type->properties[$form['#property_name']]['type']);
    if (!isset($widget_types[$form_state['values']['widget_type']])) {
      form_set_error('widget_type', t('Invalid widget.'));
    }
  }
}

/**
 * Submit callback.
 *
 * To change the widget for a property field from a bundle's field management
 * form.
 */
function eck__extra_field_widget_form_submit($form, &$form_state) {
  // Load entity_type and bundle in case they have changed since the form was
  // first built.
  $entity_type = entity_type_load($form['#entity_type']->name);
  $bundle = bundle_load($entity_type->name, $form['#bundle']->name);
  // Update the widget settings if it has changed.
  if ($form['#default_widget'] != $form_state['values']['widget_type']) {
    // Grab the config settings for this bundle so we can alter them.
    $config = (!empty($bundle->config)) ? $bundle->config : array();
    if (isset($config['extra_fields'][$form['#property_name']]['form'])) {
      $widget_info = eck_property_info_widget_types($form_state['values']['widget_type']);
      // Changing the widget so make sure to start with the default widget
      // settings.
      $widget = array(
        'type' => $form_state['values']['widget_type'],
        'settings' => $widget_info['default settings'],
      );
      $config['extra_fields'][$form['#property_name']]['form']['widget'] = $widget;
      $bundle->config = $config;
      // Save the bundle.
      $bundle->save();
      Bundle::loadAll(NULL, TRUE);
    }
    else {
      // The extra field form setting was not found in the config so it must no
      // longer be managed. Don't do anything except notify the user.
      drupal_set_message(t('Could not change the property widget. The property %name on the %bundle bundle is no selected for management.', array('%name' => $form['#property_name'], '%bundle' => $bundle->label)), 'error');
    }
  }
  $form_state['redirect'] = 'admin/structure/entity-type/' . $entity_type->name . '/' . $bundle->name . '/fields';
}

/**
 * Form for removing the property from an entity's list of extra fields.
 *
 * @param array $form
 *   The property widget form
 * @param array $form_state
 *   The current state of the form
 * @param string $entity_type
 *   The machine name of the entity type
 * @param string $bundle_type
 *   The machine name of the bundle type
 * @param string $property_name
 *   The machine name of the entity property being included as an extra field
 *
 * @return array
 *   An array comprising the form for changing the property's widget
 */
function eck__remove_extra_field_form($form, &$form_state, $entity_type, $bundle_type, $property_name) {
  $bundle = bundle_load($entity_type, $bundle_type);

  $form = array(
    '#entity_type' => $entity_type,
    '#bundle' => $bundle,
    '#property_name' => $property_name,
  );
  return confirm_form($form,
    t('Are you sure you want to remove %title as a property field for the %bundle bundle?', array('%title' => $property_name, '%bundle' => $bundle->label)),
    'admin/structure/entity-type/' . $entity_type . '/' . $bundle_type . '/properties/' . $property_name,
    t("This will remove the property as an extra field on the %bundle bundle's field management page, but it does not delete the property from the entity type. This cannot be undone.", array('%bundle' => $bundle->label)),
    t('Remove'),
    t('Cancel')
  );
}

/**
 * Callback to remove a property field from a bundle's field management form.
 *
 * The property will no longer show on the entity's creation and edit forms.
 */
function eck__remove_extra_field_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $property_name = $form['#property_name'];
    $entity_type = $form['#entity_type'];
    $bundle = bundle_load($entity_type, $form['#bundle']->name);
    if (!empty($bundle->config['extra_fields'][$property_name]['form'])) {
      $config = $bundle->config;
      // Unset the extra field form settings for this property to no longer
      // manage it.
      unset($config['extra_fields'][$property_name]['form']);
      $bundle->config = $config;
      // Save the bundle.
      $bundle->save();
      Bundle::loadAll(NULL, TRUE);
    }
  }
  $form_state['redirect'] = 'admin/structure/entity-type/' . $entity_type . '/' . $bundle->name . '/fields';
}
