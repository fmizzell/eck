<?php

/**
 * @file
 * Handles properties attached to entites created through ECK.
 */

/**
 * ECK Entity type properties page callback.
 */
function eck__properties($et_name = NULL) {
  if (empty($et_name) || !($entity_type = EntityType::loadByName($et_name))) {
    drupal_not_found();
    die;
  }
  // Prepare the content table.
  $content = array(
    '#theme' => 'table',
    '#header' => array(
      'Label',
      'Machine name',
      'Behavior',
      'Type',
      'Actions'),
    '#rows' => array(),
  );

  // Prepare some vars to reuse them in action links.
  $args = array('query' => drupal_get_destination());
  $path = "admin/structure/entity-type/{$entity_type->name}/property";
  // Loop trough properties and fill the rows.
  foreach ($entity_type->properties as $name => $info) {
    $actions = array(
      l(t('edit'), "$path/edit/{$name}", $args),
      l(t('delete'), "$path/delete/{$name}", $args),
    );
    $content['#rows'][] = array(
      $info['label'],
      $name,
      $info['behavior'],
      $info['type'],
      implode(' | ', $actions),
    );
  }

  return drupal_render($content);
}

/**
 * Form builder for property add form.
 *
 * @see eck__property_add__form_submit()
 */
function eck__property_add__form($form, &$form_state, $et_name = NULL) {
  if (empty($et_name) || ($entity_type = EntityType::loadByName($et_name)) == FALSE) {
    drupal_not_found();
    die;
  }

  // Ease access to $form_state['values'].
  $values = isset($form_state['values']) ? $form_state['values'] : array();
  // Pass the EntityType object as value.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  // Container for all entity property info.
  $form['property']['#tree'] = TRUE;
  $form['property']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
  );
  $form['property']['name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Name'),
    '#machine_name' => array(
      'source' => array('property', 'label'),
    ),
    '#element_validate' => array('_eck_property_exists'),
    '#required' => TRUE,
  );

  // Behaviors.
  $behaviors = array();
  $property_behaviors = ctools_get_plugins('eck', 'property_behavior');
  foreach ($property_behaviors as $property_behavior) {
    $behaviors[$property_behavior['name']] = $property_behavior['label'];
  }
  $form['property']['behavior'] = array(
    '#type' => 'select',
    '#title' => t('Behavior'),
    '#options' => $behaviors,
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'property-settings',
      'callback' => '_eck_property_settings_callback',
    ),
  );
  $form['property']['settings']['#prefix'] = '<div id="property-settings">';
  $form['property']['settings']['#suffix'] = '</div>';
  // Provide the property form as implemented in the behavior.
  if (isset($values['property']['behavior'])) {
    // Invoke the "property_form" plugin function and add the property form.
    $context = array(
      'entity_type' => $entity_type,
      'behavior' => $values['property']['behavior'],
      'form_state' => $form_state,
    );
    if ($property_form = eck_property_settings_form($values['property']['behavior'], $context)) {
      $form['property']['settings'] = $property_form;
      $form['property']['settings']['#type'] = 'fieldset';
      $form['property']['settings']['#title'] = t(':name behavior settings', array(':name' => $values['property']['behavior']));
    }
  }
  // Always add the property settings as an element.
  $form['property']['settings']['#tree'] = TRUE;
  $form['property']['settings']['#prefix'] = '<div id="property-settings">';
  $form['property']['settings']['#suffix'] = '</div>';
  // Property types.
  $form['property']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => eck_property_types(),
    '#required' => TRUE,
  );
  // Actions.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Ajax callback for property settings form element.
 *
 * @see eck__property_add__form()
 */
function _eck_property_settings_callback($form, $form_state) {
  return $form['property']['settings'];
}

/**
 * Element validate for property name. Must be unique per entity type.
 *
 * @see eck__property_add__form()
 */
function _eck_property_exists($element, $form_state) {
  if (empty($element['#value'])) {
    return;
  }
  $entity_type = $form_state['values']['entity_type'];
  if (isset($entity_type->properties[$element['#value']])) {
    $arg = array('%name' => $element['#value']);
    form_error($element, t('Property %name already exists. Please choose another machine name.', $arg));
  }
}

/**
 * Submit handler for property form.
 *
 * @see eck__property_add__form()
 */
function eck__property_add__form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $name = $form_state['values']['property']['name'];

  $data = $form_state['values']['property'];
  unset($data['name']);

  if ($entity_type->addProperty($name, $data) !== FALSE) {
    $entity_type->save();
    $form_state['redirect'] = "admin/structure/entity-type/{$entity_type->name}/properties";
  }
  else {
    // @TODO: woops, now what?
  }
}

/**
 * Property edit form.
 */
function eck__property_edit__form($form, $form_state, $et_name = NULL, $property_name = NULL) {
  if (empty($et_name) || !($entity_type = EntityType::loadByName($et_name)) || !isset($entity_type->properties[$property_name])) {
    drupal_not_found();
    die;
  }

  // ECK EntityType.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  // Property name.
  $form['property_name'] = array('#type' => 'value', '#value' => $property_name);

  // Wrap all property data.
  $form['property']['#tree'] = TRUE;

  $info = $entity_type->properties[$property_name];
  $form['property']['behavior'] = array('#type' => 'value', '#value' => $info['behavior']);
  $form['property']['type'] = array('#type' => 'value', '#value' => $info['type']);
  // Allow property label change.
  $form['property']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#max_lenght' => 30,
    '#default_value' => $info['label'],
  );

  // Add property settings form implemented by behavior via 'property_form'.
  $context = array(
    'property' => $property_name,
    'entity_type' => $entity_type,
    'behavior' => $info['behavior'],
    'form_state' => $form_state,
    'settings' => isset($info['settings']) ? $info['settings'] : NULL,
  );
  if ($settings_form = eck_property_settings_form($info['behavior'], $context)) {
    $form['settings'] = $settings_form;
    $form['property']['settings']['#type'] = 'fieldset';
    $form['property']['settings']['#title'] = t(':name behavior settings', array(':name' => $info['behavior']));
  }

  // Actions.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for property edit form.
 *
 * @see eck__property_edit__form()
 */
function eck__property_edit__form_submit($form, $form_state) {
  $property_name = $form_state['values']['property_name'];
  $data = $form_state['values']['property'];
  $entity_type = $form_state['values']['entity_type'];

  // Update property and save.
  if ($entity_type->addProperty($property_name, $data) !== FALSE) {
    $entity_type->save();
    drupal_set_message(t('Property %name saved.', array('%name' => $property_name)));
  }
  else {
    // @Todo: handle error.
  }
}

/**
 * DELETE: Form builder "eckextend_property_delete_form".
 *
 * @see eckextend_property_delete_form_submit()
 */
function eck__property_delete__form($form, $form_state, $et_name, $property_name) {
  if (empty($et_name) || !($entity_type = EntityType::loadByName($et_name))
      || empty($property_name) || !isset($entity_type->properties[$property_name])) {
    drupal_not_found();
    die;
  }
  $form = array();
  // Pass the entity type and property name as a values.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  $form['property_name'] = array('#type' => 'value', '#value' => $property_name);

  $args = array(
    '%label' => $entity_type->properties[$property_name]['label'],
    '%name' => $property_name,
  );
  $message = t('Are you sure you want to delete %label (%name) property?', $args);
  $caption = $message . t('This action cannot be undone.');

  return confirm_form($form, $message, "admin/confing/entity-type/{$entity_type->name}/properties", $caption, t('Delete'));
}

/**
 * DELETE: Submit handler for "eckextend_property_delete_form" form.
 *
 * @see eckextend_property_delete_form()
 */
function eck__property_delete__form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $name = $form_state['values']['property_name'];

  $entity_type->removeProperty($name);
  $entity_type->save();
}

/**
 * Invoke behavior plugin function 'property_form'.
 */
function eck_property_settings_form($behavior, $context = array()) {
  $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
  if ($function = ctools_plugin_get_function($plugin, 'property_form')) {
    $default = array(
      'entity_type' => NULL,
      'property' => NULL,
      'behavior' => $behavior,
    );
    $context = array_merge($default, $context);

    return $function($context['property'], $context);
  }
}

/**
 * Assigns fields to the db schema for an entity type.
 *
 * @param array $schema
 *   A schema array.
 */
function eck_set_properties_schema(&$schema, $entity_type) {
  $properties = $entity_type->properties;

  foreach ($properties as $name => $info) {
    $type = $info['type'];
    $schema['fields'][$name] = eck_property_type_schema($type);
  }
}

/**
 * The different property types.
 */
function eck_property_types() {
  // All types: serial, int, float, numeric, varchar, char, text,
  // blob, datetime.
  $default_types = array(
    'text' => t('Text'),
    'integer' => t('Integer'),
    'decimal' => t('Decimal'),
    'positive_integer' => t('Unsigned Integer'),
    'language' => t('Language'),
  );

  $module_default_types = module_invoke_all('eck_property_types');
  $default_types = array_merge($default_types, $module_default_types);
  drupal_alter('eck_property_types', $default_types);

  // @todo As modules start to implement ECK hooks, this should be removed.
  if (module_exists('uuid') && !array_key_exists('uuid', $default_types)) {
    $default_types['uuid'] = t('UUID');
  }

  return $default_types;
}

/**
 * The property types schemas.
 */
function eck_property_type_schema($type) {
  $schema = array();

  switch ($type) {
    case 'text':
      $schema = array(
        'description' => 'Text',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    case 'decimal':
      $schema = array(
        'description' => 'Decimal',
        'type' => 'float',
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'integer':
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'positive_integer':
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'unsigned' => TRUE,
        'default' => 0,
      );
      break;

    case 'uuid':
      $schema = array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.',
      );
      break;

    case 'language':
      $schema = array(
        'description' => 'Language',
        'type' => 'varchar',
        'length' => 12,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    default:
      $schema = NULL;
      break;
  }

  // @todo Don't really like this.
  drupal_alter('eck_property_type_schema', $schema, $type);

  return $schema;
}
