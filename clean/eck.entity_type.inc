<?php


/**
 * Entity Type specific implementation of property info alter.
 */
function eck__entity_type__property_info(&$info, $entity_type_object) {
   $properties = &$info['properties'];
  
  //@TODO Shouldn't we be checking on the properies array of our entity_type_object.. maybe entity api
  //is correctly doing this automatically (CHECK)
  if (isset($properties['uid'])) {
    $properties['uid']['label'] = t('User');
    $properties['uid']['type'] = 'user';
    $properties['uid']['description'] = t('The author of this entity.');
  }
  if (isset($properties['created'])) {
    $properties['created']['label'] = t('Created');
    $properties['created']['type'] = 'date';
    $properties['created']['description'] = t('The Unix timestamp when the entity has been created.');
  }
  if (isset($properties['changed'])) {
    $properties['changed']['label'] = t('Changed');
    $properties['changed']['type'] = 'date';
    $properties['changed']['description'] = t('The Unix timestamp when the entity was most recently saved.');
  }

  // Add custom properties.
  foreach ($entity_type_object->custom_properties as $name => $property) {
    $properties[$name]['label'] = t($property['label']);
    $properties[$name]['type'] = $property['type'];
    //$properties[$name]['description'] = '';
  }
}

function eck__entity_type__custom_property_label($property) {
  $type = $property['type'];
  if (!in_array($type, array('text', 'decimal', 'integer', 'date'))) {
    $type = l($type, "admin/structure/eck/{$type}");
  }
  return $property['label'] . ' (' . t('type: !type', array('!type' => $type)) . ')';
}

/**
 * Delete the entity type
 * 
 * @param $entity_type 
 *  (EntityType) entity type to be deleted, the $entity_type object can be loaded by using the 
 *  method EntityType::loadByName($entity_type_name).
 * 
 * for more information on the EntityType class look at eck.classes.inc 
 */
function eck__entity_type__delete($entity_type){
  
  module_load_include('inc', 'eck', 'eck.bundle');
  //delete all the bundles from this entity type
  $bundles = Bundle::loadByEntityType($entity_type);
  foreach($bundles as $bundle){
    eck__bundle__delete($entity_type, $bundle);
  }
  
  $entity_type->delete();
 
  drupal_set_message("Entity type '{$entity_type->name}' has been deleted");
  //return "<h1>Deletion Completed</h1> <h3>Entity type '{$entity_type}' has been deleted";
}

/**
 * When an entity type is being addes, we need to make sure that
 * its name is unique.
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 */
function eck__entity_type__form_validate($form, &$state) {
  if ($form['entity_type']['#value']->is_new) {
    $info = entity_get_info();
    $entity_types = array_keys($info);
    $entity_type = $state['values']['entity_type_name'];
    if (in_array($entity_type, $entity_types)) {
      form_set_error('name', t("Entity Type {$entity_type} already exists"));
    }
  }
}





