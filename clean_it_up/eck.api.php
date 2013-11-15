<?php


/**
 * Respond to the creation of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being created.
 */
function hook_eck_entity_type_insert(EntityType $entity_type) {

}

/**
 * Respond to the updating of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being update.
 */
function hook_eck_entity_type_update(EntityType $entity_type) {

}

/**
 * Respond to the deletion of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being deleted.
 */
function hook_eck_entity_type_delete(EntityType $entity_type) {

}

/**
 * Default properties, are prebuild properties with a schema and a behavior
 */
function hook_eck_default_properties(){
  
  $default_properties['property_machine_name'] =
  array(
    'label' => "Property Label",
    'type' => "property_type",
    'behavior' => 'property_behavior'
  );
  
  return $default_properties;
}
