<?php

/**
 * Implements hook_field_extra_fields().
 */
function eck_field_extra_fields() {
  $extra = array();

  foreach(EntityType::loadAll() as $entity_type){
    foreach(Bundle::loadByEntityType($entity_type) as $bundle){
      foreach($entity_type->properties as $property_name => $property_info) {
        if (!empty($bundle->config['extra_fields'][$property_name]['form'])) {
          $extra[$entity_type->name][$bundle->name]['form'][$property_name] = array(
            'label' => $bundle->config['extra_fields'][$property_name]['form']['label'],
            'description' => t('Entity property: %type', array('%type' => $property_info['type'])),
            'weight' => 0,
          );
        }
        if (!empty($bundle->config['extra_fields'][$property_name]['display'])) {
          $extra[$entity_type->name][$bundle->name]['display'][$property_name] = array(
            'label' => $bundle->config['extra_fields'][$property_name]['display']['label'],
            'description' => t('Entity property: %type', array('%type' => $property_info['type'])),
            'weight' => 0,
          );
        }
      }
    }
  }
  return $extra;
}
