<?php
/**
 * @file
 * Default properties are a grouping of a property and a behavior.
 */

/**
 * This is a default property.
 */
function eck__is_default_property($property) {
  $default_properties = eck_get_default_properties();
  $is_default = FALSE;

  if (array_key_exists($property, $default_properties)) {
    $is_default = TRUE;
  }

  return $is_default;
}

/**
 * Activate default properties.
 */
function eck__activate_default_property($entity_type_name, $property) {
  $entity_type = EntityType::loadByName($entity_type_name);
  if (eck__is_default_property($property)) {
    // We have 2 cases, if it is active, do nothing.
    $properties = $entity_type->properties;
    if (array_key_exists($property, $properties)) {
      drupal_set_message("{$property} is already active");
    }
    // Else activate the property.
    else {
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
      $entity_type->save();
      EntityType::loadAll(NULL, TRUE);
    }
  }
  else {
    drupal_set_message("{$property} is not a default property", "error");
  }
  // @todo more uses of goto??
  drupal_goto(eck__entity_type__path() . "/{$entity_type->name}/property");
}

/**
 * Deactivate default properties.
 */
function eck__deactivate_default_property($entity_type_name, $property) {
  $entity_type = EntityType::loadByName($entity_type_name);
  if (eck__is_default_property($property)) {
    // We have 2 cases, if it is active, do nothing.
    $properties = $entity_type->properties;
    if (!array_key_exists($property, $properties)) {
      drupal_set_message("{$property} is already inactive");
    }
    // Else activate the property.
    else {
      $entity_type->removeProperty($property);
      $entity_type->save();
      EntityType::loadAll(NULL, TRUE);
    }
  }
  else {
    drupal_set_message("{$property} is not a default property", "error");
  }
  // @todo if we can not use drupal goto, that could be better.
  drupal_goto(eck__entity_type__path() . "/{$entity_type->name}/property");
}

/**
 * Implements hook_form().
 */
function eck__default_properties__form($form, &$state, $entity_type) {
  // DEFAULT PROPERTIES.
  // Now we want to display the default properties.

  $default_properties = eck_get_default_properties();
  $options = array();
  foreach ($default_properties as $property_name => $property_info) {
    $options[$property_name] = $property_info['label'];
  }

  $form['default_properties'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => t('Default Properties'),
  );

  $properties = $entity_type->properties;

  $defaults = array();
  foreach ($properties as $property => $info) {
    $defaults[$property] = $property;
  }

  $form['default_properties']['#default_value'] = $defaults;
  // :S

  return $form;
}

/**
 * Form submition callback for default properties.
 */
function eck__default_properties__form_submit($form, &$state, $entity_type) {

  $entity_type = $state['values']['entity_type'];
  $dp = $state['values']['default_properties'];
  foreach ($dp as $property => $active) {
    if ($active) {
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
    }
    else {
      $entity_type->removeProperty($property);
    }
  }

  $state['values']['entity_type'] = $entity_type;
}

/**
 * All default properties information.
 * @todo combine this and get_default_property().
 *
 * @return array
 *   All of the default properties info.
 */
function eck_get_default_properties() {
  return module_invoke_all('eck_default_properties');
}

/**
 * Get a default property by name.
 *
 * @param string $name
 *   Default property's name.
 *
 * @return array
 *   The default property info.
 */
function eck_get_default_property($name) {
  $default_properties = eck_get_default_properties();
  return $default_properties[$name];
}
