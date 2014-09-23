<?php
/**
 * @file
 * UUID behavior definition.
 */

if (module_exists('uuid')) {
  $plugin = array(
    'label' => 'UUID',
    'entity_info' => 'eck_uuid_property_entity_info',
    'entity_save' => 'eck_uuid_property_entity_insert',
    'property_info' => 'eck_uuid_property_property_info',
  );
}

/**
 * Connect this entity type to the UUID system through entity_info.
 */
function eck_uuid_property_entity_info($property, $var) {
  $info = $var;
  // Put UUID into entity keys.
  $info['entity keys']['uuid'] = $property;
  $info['uuid'] = TRUE;
  return $info;
}

/**
 * Generate the UUID on entity insertion.
 */
function eck_uuid_property_entity_insert($property, $vars) {
  if (module_exists('uuid')) {
    $entity = $vars['entity'];
    if (empty($entity->{$property})) {
      $entity->{$property} = uuid_generate();
    }
  }
}

/**
 * Metadata about our property.
 */
function eck_uuid_property_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'text';
  $vars['properties'][$property]['description'] = t('The universally unique ID.');
  return $vars;
}
